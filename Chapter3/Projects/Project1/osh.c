#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>

#include "osh.h"

static int exec_cmd(Command* cmd);

void print_cmd(const Command *cmd) {
	if(cmd && cmd->argc) {
		printf("%s", cmd->args[0]);

		for(size_t i = 1; i < cmd->argc; ++i)
			printf(" %s", cmd->args[i]);

		if(cmd->in)
			printf(" < %s", cmd->in);
		if(cmd->out)
			printf(" > %s", cmd->out);
		if(cmd->bg)
			printf(" &");
	}
}

void print_cmds(const Command **cmds, size_t cmdc) {
	if(cmds && cmdc) {
		print_cmd(cmds[0]);
		for(size_t i = 1; i < cmdc; ++i) {
			printf(" | ");
			print_cmd(cmds[i]);
		}
	}
	printf("\n");
}

Command** allocate_cmd_buffer() {
	Command **cmds = (Command **)calloc(MAX_CMDC, sizeof(Command*));
	for(size_t i = 0; i < MAX_CMDC; ++i){
		cmds[i] = (Command*)malloc(sizeof(Command));
		if(!cmds[i])
			return NULL;

		for(size_t j = 0; j < MAX_ARGC; ++j) {
			cmds[i]->args[j] = (char *)calloc(MAX_LEN + 1, sizeof(char));
			if(!cmds[i]->args[j])
				return NULL;
		}
	}

	return cmds;
}

void free_cmd_buffer(Command **cmds) {
	if(cmds) {
		for(size_t i = 0; i < MAX_CMDC; ++i){
			Command *cmd = cmds[i];
			for(size_t j = 0; j < MAX_ARGC; ++j) {
				free(cmd->args[j]);
			}
			free(cmd);
		}
		free(cmds);
	}
}

Command ** copy_cmds(Command **dst_cmds, const Command **src_cmds, size_t cmdc) {
	if(!dst_cmds || !src_cmds)
		return NULL;

	for(size_t i = 0; i < cmdc; ++i) {
		Command *dst_cmd = dst_cmds[i];
		const Command* src_cmd = src_cmds[i];
		size_t n = src_cmd->argc;

		for(size_t j = 0; j < src_cmd->argc; ++j)
			strcpy(dst_cmd->args[j], src_cmd->args[j]);

		if(src_cmd->out && src_cmd->in) {
			/*
				Use the already allocated buffers to copy the filenames.
				Since dst_cmd->args[n] will be NULLed in exec_cmd(),
				use the locations after it
			*/
			dst_cmd->in = dst_cmd->args[n+1];
			strcpy(dst_cmd->in, src_cmd->in);

			dst_cmd->out = dst_cmd->args[n+2];
			strcpy(dst_cmd->out, src_cmd->out);
		}
		else if(src_cmd->in) {
			dst_cmd->out = NULL;
			dst_cmd->in = dst_cmd->args[n+1];
			strcpy(dst_cmd->in, src_cmd->in);
		}
		else if(src_cmd->out) {
			dst_cmd->in = NULL;
			dst_cmd->out = dst_cmd->args[n+1];
			strcpy(dst_cmd->out, src_cmd->out);
		}
		else {
			dst_cmd->in = NULL;
			dst_cmd->out = NULL;
		}

		dst_cmd->argc = n;
	}

	return dst_cmds;
}

int parse_cmd_line(char line[], Command** cmds) {
	if(!cmds)
		return -1;

	size_t cmdc = 0, argc = 0;
	char *saveptr1, *saveptr2;
	char *s = strtok_r(line, "|", &saveptr1);
	while(s && cmdc < MAX_CMDC) {
		argc = 0;
		Command *cmd = cmds[cmdc];
		char *e = strtok_r(s, " ", &saveptr2);
		while(e && argc < MAX_ARGC) {
			strncpy(cmd->args[argc++], e, MAX_LEN);
			e = strtok_r(NULL, " ", &saveptr2);
		}

		cmd->in = NULL;
		cmd->out = NULL;

		size_t i = argc && !strcmp("&", cmd->args[argc-1]); /* flag to determine a background command */
		size_t n = i; /* Number of arguments to be discarded */
		if(argc >= 3+i) { /* the command may contain input/output redirection */
			char c = cmd->args[argc-2-i][0];
			if(c == '<') {
				cmd->in = cmd->args[argc-1-i];
				n += 2;
			}
			else if(c == '>') {
				cmd->out = cmd->args[argc-1-i];
				n += 2;
			}

			if(argc >= 5+i) {
				char c = cmd->args[argc-4-i][0];
				if(c == '<' && !cmd->in && cmd->out) {
					cmd->in = cmd->args[argc-3-i];
					n += 2;
				}
				else if(c == '>' && !cmd->out && cmd->in) {
					cmd->out = cmd->args[argc-3-i];
					n += 2;
				}
			}
		}

		argc -= n;
		cmd->bg = i;
		cmd->argc = argc;
		++cmdc;

		s = strtok_r(NULL, "|", &saveptr1);
	}

	return cmdc;
}

int execute(Command** cmds, size_t cmdc) {

	if(!cmds || cmdc == 0)
		return -1;

	if(cmdc > 1) {
		int fds[2];
		if(pipe(fds) < 0) {
			fprintf(stderr, "%s: %s\n", __func__, strerror(errno));
			return -1;
		}

		pid_t pid = fork();
		if(pid < 0) {
			fprintf(stderr, "%s: %s\n", __func__, strerror(errno));
			return -1;
		}
		else if(pid == 0) {
			close(fds[1]);	/* close write end */
			dup2(fds[0], STDIN_FILENO);	/* redirect stdin to the pipe's read end */
			return execute(&cmds[1], cmdc - 1);
		}
		else {
			close(fds[0]);	/* close read end */
			dup2(fds[1], STDOUT_FILENO);	/* redirect stdout to the pipe's write end */
			return exec_cmd(cmds[0]);
		}
	}
	else
		return exec_cmd(cmds[0]);
}

static int exec_cmd(Command* cmd) {
	if(!cmd)
		return -1;

	/* redirect input */
	if(cmd->in) {
		int fd = open(cmd->in, O_RDONLY);
		if(fd < 0) {
			fprintf(stderr, "%s: %s\n", __func__, strerror(errno));
			return -1;
		}
		dup2(fd, STDIN_FILENO);
	}

	/* redirect output */
	if(cmd->out) {
		int fd = open(cmd->out, O_WRONLY | O_TRUNC | O_CREAT, 0666);
		if(fd < 0) {
			fprintf(stderr, "%s: %s\n", __func__, strerror(errno));
			return -1;
		}
		dup2(fd, STDOUT_FILENO);
	}

	cmd->args[cmd->argc] = NULL; /* terminate the command vector */
	if(execvp(cmd->args[0], cmd->args) < 0) {
		fprintf(stderr, "%s: %s\n", __func__, strerror(errno));
		return -1;
	}
}
