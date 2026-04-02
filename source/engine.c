#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "engine.h"

static void render_markdown_chunk(const char *data, size_t len) {
    static int code_mode = 0;
    static int bold_mode = 0;

    for (size_t i = 0; i < len; i++) {
        if (data[i] == '#' && (i == 0 || (i > 0 && data[i-1] == '\n'))) {
            printf("\033[1;36m");
            putchar(data[i]);
        } 
        else if (data[i] == '`') {
            if (!code_mode) {
                printf("\033[40;33m");
                code_mode = 1;
            } else {
                printf("\033[0m");
                code_mode = 0;
            }
        }
        else if (data[i] == '*' && i + 1 < len && data[i+1] == '*') {
            if (!bold_mode) {
                printf("\033[1m");
                bold_mode = 1;
            } else {
                printf("\033[0m");
                bold_mode = 0;
            }
            i++; 
        }
        else {
            putchar(data[i]);
        }
    }
}

static size_t write_to_stdout(void *contents, size_t size, size_t nmemb, void *userp) {
    (void)userp;
    size_t realsize = size * nmemb;
    render_markdown_chunk((const char *)contents, realsize);
    return realsize;
}

void run_engine(const char *url) {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_stdout);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L); 

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            printf("Error: %s\n", curl_easy_strerror(res));
        }
		fprintf(stdout, "\n");
        curl_easy_cleanup(curl);
    }
}