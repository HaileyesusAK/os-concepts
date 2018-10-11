#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length command */

int get_words(char line[], char * words[], size_t max_count) {
	if(!words)
		return -1;

	int i = 0;
	char *s = strtok(line, " ");
	while(s && i < max_count) {
		words[i++] = s;
		s = strtok(NULL, " ");
	}
	words[i] = NULL;

	return i;
}

int main(void)
{
	char *args[MAX_LINE/2 + 1];		/* command line arguments */
	int should_run = 1;				/* flag to determine when to exit program */
	char command_line[MAX_LINE];	/* unparsed command line */
	int argc = 0;

	while (should_run) {
		/* read user input */
		printf("osh>");
		fflush(stdout);
		fgets((char*)command_line, MAX_LINE + 1, stdin);
		command_line[strlen(command_line)-1] = '\0';	/* replace the new-line by a terminator */

		argc = get_words(command_line, args, MAX_LINE/2);
		if(argc < 0) {
			fprintf(stderr, "Error parsing '%s'", command_line);
			continue;
		}
		else if(argc > 0) {
			if(strcmp("exit", args[0]) == 0)
				should_run = 0;
			else {	/* execute the command*/

				int bg = !strcmp(args[argc - 1], "&");

				pid_t pid = fork();
				if(pid < 0) {
					perror("fork");
				}
				else if(pid == 0) { /* child process */
					if(bg)
						args[argc - 1] = NULL;	/* remove the trailing ampersand */

					if(execvp(args[0], args) < 0) {
						perror("");
						return EXIT_FAILURE;
					}
				}
				else { /* parent process */
					if(!bg)
						wait(NULL);
				}
			}
		}
	}

	return 0;
}
