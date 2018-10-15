#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include "utils.h"

#define MAX_CMD_LEN 80						/* The maximum length args */
#define MAX_CMD_CNT (MAX_CMD_LEN/2)			/* The maximum number of commands */

int main(void)
{
	char *args[MAX_CMD_CNT + 1];			/* parsed command line arguments */
	int should_run = 1;						/* flag to determine when to exit program */
	char cmd_line[MAX_CMD_LEN + 1];			/* unparsed command line */
	char recent_cmd_line[MAX_CMD_LEN + 1];	/* most recent unparsed command line */
	int argc = 0;

	/* allocate memory to hold parsed args */
	if(allocate_cmd_buffer(args, MAX_CMD_CNT, MAX_CMD_LEN) < 0) {
	    fprintf(stderr, "Failed to allocate memory\n");
	    return EXIT_FAILURE;
	}

	while (should_run) {
		/* read user input */
		printf("osh>");
		fflush(stdout);
		fgets((char*)cmd_line, MAX_CMD_LEN + 1, stdin);
		cmd_line[strlen(cmd_line)-1] = '\0';	/* replace the new-line by a terminator */

		argc = get_words(cmd_line, args, MAX_CMD_CNT);
		if(argc < 0) {
			fprintf(stderr, "Error parsing '%s'", cmd_line);
			continue;
		}
		else if(argc > 0) {
			if(strcmp("exit", args[0]) == 0)
				should_run = 0;
			else {
				/* execute the most recent command if required and if it exists */
				if(strcmp("!!", args[0]) == 0) {
					if(sizeof(recent_cmd_line)) {
						argc = get_words(recent_cmd_line, args, MAX_CMD_CNT);
						print_cmd(args, argc);
					}
					else {
						fprintf(stderr, "No cmds in history\n");
						continue;
					}
				}
				else {
				    /* memorize the current command */
					strcpy(recent_cmd_line, cmd_line);
				}

				int bg = !strcmp(args[argc - 1], "&");

				pid_t pid = fork();
				if(pid < 0) {
					perror("fork");
				}
				else if(pid == 0) { /* child process */
					int i = -1;	// Index of either > or < in the args vector
					if(bg)
						i = argc - 3;
					else
						i = argc - 2;

					/* Redirect input or output */
					if(i > 0) {
						int fd = -1;
						printf("%s %s %s\n", args[i - 1], args[i], args[i+1]);
						if(strcmp("<", args[i]) == 0) {
							fd = open(args[i + 1], O_RDONLY);
							if(fd < 0) {
								fprintf(stderr, "error opening %s: %s\n", args[i + 1], strerror(errno));
								return EXIT_FAILURE;
							}
							dup2(fd, STDIN_FILENO);
						}
						else if(strcmp(">", args[i]) == 0) {
							fd = open(args[i + 1], O_WRONLY | O_TRUNC | O_CREAT, 0666);
							if(fd < 0) {
								fprintf(stderr, "error opening %s: %s\n", args[i + 1], strerror(errno));
								return EXIT_FAILURE;
							}
							dup2(fd, STDOUT_FILENO);
						}
						else
							i = bg ? argc - 1: argc;
					}
					else
						i = bg ? argc - 1 : argc;

					args[i] = NULL;	/* terminate the command vector */

					if(execvp(args[0], args) < 0) {
						fprintf(stderr, "%s: %s\n", args[0], strerror(errno));
						return EXIT_FAILURE;
					}
				}
				else { /* parent process */
					if(!bg)	/* wait for child process if the command is not a background job */
						wait(NULL);
				}
			}
		}
	}

    free_cmd_buffer(args, MAX_CMD_CNT);

	return 0;
}
