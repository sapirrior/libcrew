#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "engine.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("Error: Missing arguments\n");
        return 1;
    }

    char category[32] = "pkg";
    char item[64];
    char final_url[512];

    char *colon = strchr(argv[1], ':');

    if (colon) {
        size_t type_len = colon - argv[1];
        if (type_len < sizeof(category)) {
            strncpy(category, argv[1], type_len);
            category[type_len] = '\0';
        }
        strncpy(item, colon + 1, sizeof(item) - 1);
        item[sizeof(item) - 1] = '\0';
    } else {
        strncpy(item, argv[1], sizeof(item) - 1);
        item[sizeof(item) - 1] = '\0';
    }

    snprintf(final_url, sizeof(final_url), 
             "https://raw.githubusercontent.com/sapirrior/libcrew/main/registry/%s/%s.md", 
             category, item);

    run_engine(final_url);

    return 0;
}