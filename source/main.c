#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "engine.h"

#define LIST	"\033[38;2;128;128;128m"
#define OFF	"\033[0m"

int main(int argc, char *argv[]) {
	char	category[32] = "pkg";
	char	item[64];
	char	label[96];
	char	url[512];

	if (argc < 2) {
		fprintf(stderr, LIST "usage: icrawl [category:]<item>\n" OFF);
		return 1;
	}

	char *colon = strchr(argv[1], ':');

	if (colon) {
		size_t type_len = (size_t)(colon - argv[1]);
		if (type_len >= sizeof(category)) {
			fprintf(stderr, LIST "error: category too long\n" OFF);
			return 1;
		}
		strncpy(category, argv[1], type_len);
		category[type_len] = '\0';
		strncpy(item, colon + 1, sizeof(item) - 1);
		item[sizeof(item) - 1] = '\0';
	} else {
		strncpy(item, argv[1], sizeof(item) - 1);
		item[sizeof(item) - 1] = '\0';
	}

	if (item[0] == '\0') {
		fprintf(stderr, LIST "error: empty item name\n" OFF);
		return 1;
	}

	snprintf(label, sizeof(label), "%s:%s", category, item);
	snprintf(url, sizeof(url),
		"https://raw.githubusercontent.com/sapirrior/icrawl/main/registry/%s/%s.md",
		category, item);

	run_engine(url, label);

	return 0;
}
