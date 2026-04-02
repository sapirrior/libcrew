#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "engine.h"

#define H1	"\033[38;2;240;145;95m"
#define H2	"\033[38;2;210;115;72m"
#define H3	"\033[38;2;175;90;55m"
#define CODE	"\033[38;2;110;110;110m"
#define LIST	"\033[38;2;128;128;128m"
#define RULE	"\033[38;2;100;100;100m"
#define BOLD	"\033[1;38;2;217;119;87m"
#define RESET	"\033[38;2;255;255;255m"
#define OFF	"\033[0m"

typedef struct {
	int	code_block;
	int	inline_code;
	int	bold_mode;
	char	carry;
	char	line_buf[4096];
	size_t	line_len;
} RenderState;

static void emit_carry(RenderState *s) {
	if (s->carry) {
		putchar(s->carry);
		s->carry = 0;
	}
}

static void render_inline(RenderState *s, const char *line, size_t len) {
	for (size_t i = 0; i < len; i++) {
		char c = line[i];

		if (c == '`') {
			if (!s->inline_code) {
				emit_carry(s);
				printf(CODE);
				s->inline_code = 1;
			} else {
				printf(RESET);
				s->inline_code = 0;
			}
			continue;
		}

		if (c == '*' && !s->inline_code) {
			if (s->carry == '*') {
				if (!s->bold_mode) {
					printf(BOLD);
					s->bold_mode = 1;
				} else {
					printf(RESET);
					s->bold_mode = 0;
				}
				s->carry = 0;
			} else {
				emit_carry(s);
				s->carry = '*';
			}
			continue;
		}

		emit_carry(s);
		putchar(c);
	}

	emit_carry(s);
	if (s->inline_code) { printf(RESET); s->inline_code = 0; }
	if (s->bold_mode)   { printf(RESET); s->bold_mode = 0; }
}

static void render_line(RenderState *s, const char *line, size_t len) {
	if (len == 0) {
		putchar('\n');
		return;
	}

	if (s->code_block) {
		if (len >= 3 && strncmp(line, "```", 3) == 0) {
			printf(OFF "\n");
			s->code_block = 0;
		} else {
			printf(CODE);
			fwrite(line, 1, len, stdout);
			putchar('\n');
		}
		return;
	}

	if (len >= 3 && strncmp(line, "```", 3) == 0) {
		s->code_block = 1;
		return;
	}

	if (len >= 3 && strncmp(line, "---", 3) == 0) {
		printf(RULE);
		for (int i = 0; i < 48; i++) putchar('-');
		printf(OFF "\n");
		return;
	}

	int h_level = 0;
	while (h_level < (int)len && line[h_level] == '#') h_level++;
	if (h_level > 0 && h_level <= 3 && h_level < (int)len && line[h_level] == ' ') {
		const char *hc = h_level == 1 ? H1 : h_level == 2 ? H2 : H3;
		const char *text = line + h_level + 1;
		size_t text_len = len - (size_t)(h_level + 1);
		printf("%s", hc);
		fwrite(text, 1, text_len, stdout);
		printf(OFF "\n");
		return;
	}

	if (len >= 2 && (line[0] == '-' || line[0] == '*') && line[1] == ' ') {
		printf(LIST "  \xe2\x80\xa2 " RESET);
		render_inline(s, line + 2, len - 2);
		printf(OFF "\n");
		return;
	}

	if (len >= 3 && line[0] >= '1' && line[0] <= '9' && line[1] == '.' && line[2] == ' ') {
		printf(LIST);
		putchar(line[0]);
		printf(". " RESET);
		render_inline(s, line + 3, len - 3);
		printf(OFF "\n");
		return;
	}

	printf(RESET);
	render_inline(s, line, len);
	printf(OFF "\n");
}

static void process_bytes(RenderState *s, const char *data, size_t len) {
	for (size_t i = 0; i < len; i++) {
		if (data[i] == '\n') {
			render_line(s, s->line_buf, s->line_len);
			s->line_len = 0;
		} else {
			if (s->line_len + 1 < sizeof(s->line_buf)) {
				s->line_buf[s->line_len++] = data[i];
			} else {
				render_line(s, s->line_buf, s->line_len);
				s->line_len = 0;
				s->line_buf[s->line_len++] = data[i];
			}
		}
	}
}

static size_t write_cb(void *contents, size_t size, size_t nmemb, void *userp) {
	size_t		realsize = size * nmemb;
	RenderState	*s = (RenderState *)userp;
	process_bytes(s, (const char *)contents, realsize);
	fflush(stdout);
	return realsize;
}

void run_engine(const char *url) {
	CURL		*curl;
	CURLcode	res;
	RenderState	state;

	memset(&state, 0, sizeof(RenderState));

	curl = curl_easy_init();
	if (!curl)
		return;

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &state);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "crew/1.0");

	res = curl_easy_perform(curl);

	if (state.line_len > 0)
		render_line(&state, state.line_buf, state.line_len);

	if (res != CURLE_OK)
		fprintf(stderr, LIST "error: %s\n" OFF, curl_easy_strerror(res));

	fprintf(stdout, OFF "\n");
	curl_easy_cleanup(curl);
}
