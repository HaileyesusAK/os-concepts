#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define LOWER_CASE_ASCII_MIN 97
#define LOWER_CASE_ASCII_MAX 123
#define UPPER_CASE_ASCII_MIN 65
#define UPPER_CASE_ASCII_MAX 91
#define MAX_SIZE 1024

int main(int argc, char* argv[]) {
	if(argc < 2) {
		fprintf(stderr, "Usage: %s <string>\n", argv[0]);
		return EXIT_FAILURE;
	}

	int pipefd[2];
	if(pipe(pipefd) < 0) {
		perror("pipe");
		return EXIT_FAILURE;
	}

	pid_t pid = fork();
	if(pid < 0) {
		perror("fork");
		return EXIT_FAILURE;
	}
	else if(pid == 0) { /* child process*/
		char buf[MAX_SIZE];
		size_t count = 0;
		close(pipefd[0]);
		/* reverse the case of each command line arguments and write it onto the pipe */
		for(int i = 1; i < argc; ++i) {
			count = strlen(argv[i]);
			for(int j = 0; j < count; ++j) {
				char c = argv[i][j];
				if(LOWER_CASE_ASCII_MIN <= c && c <= LOWER_CASE_ASCII_MAX)
					buf[j] = c - 32;
				else if(UPPER_CASE_ASCII_MIN <= c && c <= UPPER_CASE_ASCII_MAX)
					buf[j] = c + 32;
				else
					buf[j] = c;
			}

			if(write(pipefd[1], buf, count) < 0) {
				perror("write");
				return EXIT_FAILURE;
			}
		}
		close(pipefd[1]);
	}
	else { /* parent process */
		char buf[MAX_SIZE];
		size_t count;
		close(pipefd[1]);
		/* read each reversed word and print it to std output */
		for(int i = 1; i < argc; ++i) {
			count = strlen(argv[i]);
			if(read(pipefd[0], buf, count) < 0) {
				perror("read");
				return EXIT_FAILURE;
			}

			buf[count] = '\0';
			printf("%s ", buf);
		}
		close(pipefd[0]);
		printf("\n");
	}

	return EXIT_SUCCESS;
}
