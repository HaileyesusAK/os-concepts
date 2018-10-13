#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include "utils.h"

#define MAX_CMD_LEN 80						/* The maximum length command */
#define MAX_CMD_CNT (MAX_CMD_LEN/2)			/* The maximum number of commands */

int main(void)
{
	char *curr_cmd[MAX_CMD_CNT + 1];		/* command line arguments */
	char *recent_cmd[MAX_CMD_CNT + 1];		/* most recent command line arguments */
	char **cmd;                             /* A pointer to the current command or the most recent command */
	int should_run = 1;						/* flag to determine when to exit program */
	char command_line[MAX_CMD_LEN];			/* unparsed command line */
	int argc = 0;
	int recent_argc = 0;

	/* allocate memory to hold the most recent command */
	if(allocate_cmd_buffer(recent_cmd, MAX_CMD_CNT, MAX_CMD_LEN) < 0) {
	    fprintf(stderr, "Failed to allocate memory\n");
	    return EXIT_FAILURE;
	}

	while (should_run) {
		/* read user input */
		printf("osh>");
		fflush(stdout);
		fgets((char*)command_line, MAX_CMD_LEN + 1, stdin);
		command_line[strlen(command_line)-1] = '\0';	/* replace the new-line by a terminator */

		argc = get_words(command_line, curr_cmd, MAX_CMD_CNT);
		if(argc < 0) {
			fprintf(stderr, "Error parsing '%s'", command_line);
			continue;
		}
		else if(argc > 0) {
			if(strcmp("exit", curr_cmd[0]) == 0)
				should_run = 0;
			else {
				/* execute the most recent command if required and if it exists */
				if(strcmp("!!", curr_cmd[0]) == 0) {
					if(recent_argc) {
					    cmd = recent_cmd;
					    argc = recent_argc;
						print_cmd(recent_cmd, argc);
					}
					else {
						fprintf(stderr, "No commands in history\n");
						continue;
					}
				}
				else {
				    /* memorize the current command */
				    copy_cmd(recent_cmd, curr_cmd, argc);
				    recent_argc = argc;
				    cmd = curr_cmd;
				}

				int bg = !strcmp(cmd[argc - 1], "&");

				pid_t pid = fork();
				if(pid < 0) {
					perror("fork");
				}
				else if(pid == 0) { /* child process */
					/* terminate the command vector */
					if(bg)
						cmd[argc - 1] = NULL;	/* overwrite the trailing ampersand */
					else
						cmd[argc] = NULL;

					if(execvp(cmd[0], cmd) < 0) {
						fprintf(stderr, "%s: %s\n", cmd[0], strerror(errno));
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

    free_cmd_buffer(recent_cmd, MAX_CMD_CNT);

	return 0;
}
