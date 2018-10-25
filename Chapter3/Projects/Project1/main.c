#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>
#include <errno.h>

#include "osh.h"

int main(void)
{
	int should_run = 1;							/* flag to determine when to exit program */
	char cmd_line[MAX_LEN * MAX_CMDC + 1];		/* unparsed command line */
	Command **cmds;								/* A pointer to the command to be executed */
	size_t cmdc = 0;
	size_t recent_cmdc = 0;

	Command** curr_cmds = allocate_cmd_buffer();
	if(!curr_cmds) {
	    fprintf(stderr, "Failed to allocate memory\n");
	    return EXIT_FAILURE;
	}

	Command** recent_cmds = allocate_cmd_buffer();
	if(!recent_cmds) {
	    fprintf(stderr, "Failed to allocate memory\n");
	    return EXIT_FAILURE;
	}

	while (should_run) {
		/* read user input */
		printf("osh>");
		fflush(stdout);
		fgets((char*)cmd_line, MAX_LEN * MAX_CMDC + 1, stdin);

		/* replace the new-line by a terminator */
		char *e = strchr(cmd_line, '\n');
		if(e)
			*e = '\0';

		cmdc = parse_cmd_line(cmd_line, curr_cmds);

		if(cmdc < 0) {
			fprintf(stderr, "error parsing '%s'", cmd_line);
			continue;
		}
		else if(cmdc > 0) {
			if(strcmp("exit", curr_cmds[0]->args[0]) == 0)
				should_run = 0;
			else {
				if(strcmp("!!", curr_cmds[0]->args[0]) == 0) {
					if(recent_cmdc) {
						cmds = recent_cmds;
						cmdc = recent_cmdc;
						print_cmds(cmds, cmdc);
					}
					else {
						fprintf(stderr, "No commands in history\n");
						continue;
					}
				}
				else {	/* save the current command */
					copy_cmds(recent_cmds, curr_cmds, cmdc);
					recent_cmdc = cmdc;
					cmds = curr_cmds;
				}

				pid_t pid = fork();
				if(pid < 0) {
					perror("fork");
				}
				else if(pid == 0) { /* child process */
					int ret = execute(cmds, cmdc);
					if(ret < 0) {
						fprintf(stderr, "%s: %d\n", cmd_line, ret);
						return EXIT_FAILURE;
					}
				}
				else { /* parent process */
					if(!cmds[cmdc-1]->bg)	/* wait for child if the command is not a background */
						wait(NULL);
				}
			}
		}
	}

	return 0;
}
