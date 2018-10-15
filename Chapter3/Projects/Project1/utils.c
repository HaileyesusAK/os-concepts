#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "utils.h"

size_t get_words(char line[], char * words[], size_t max_count) {
	if(!words)
		return -1;

	size_t i = 0;
	char *s = line;
	char *e = s;
	while(i < max_count && *s) {
		while(isspace(*s))
			++s;

		if(*s) { /* If the string is not exhausted */
			e = s;
			while(isgraph(*e))
				++e;

			snprintf(words[i++], e - s + 1, "%s", s);
			s = e;
		}
	}

	return i;
}

int allocate_cmd_buffer(char *args[], size_t argc, size_t argl) {
	if(!args)
		return -1;

	for(size_t i = 0; i < argc; ++i){
		if(!(args[i] = (char*)calloc(argl, sizeof(char)))) {
			return -2;
		}
	}

	return 0;
}

int free_cmd_buffer(char *args[], size_t argc) {
	if(!args)
		return -1;

	for(size_t i = 0; i < argc; ++i){
	    if(args[i] != NULL) {
		    free(args[i]);
		    args[i] = NULL;
	    }
	}

	return 0;
}

int copy_cmd(char *dst_args[], char * const src_args[], size_t argc) {
    if(!dst_args || !src_args)
        return -1;

    for(size_t i = 0; i < argc; ++i)
        strcpy(dst_args[i], src_args[i]);

    return 0;
}

int print_cmd(char * const cmd[], size_t argc) {
	if(!cmd)
		return -1;

    for(size_t i = 0; i < argc; ++i)
        printf("%s ", cmd[i]);

	printf("\n");
	return 0;
}
