#ifndef __OSH_H__

#define MAX_LEN 80						/* maximum length args */
#define MAX_ARGC (MAX_LEN/2)			/* maximum number of arguments per command */
#define MAX_CMDC 10						/* maximum number of piped commands */

typedef struct {
	char *args[MAX_ARGC + 1];
	char *in;		/* a pointer to the filename for INPUT redirection */
	char *out;		/* a pointer to the filename for OUTPUT redirection */
	char bg;		/* a flag to determine weather the command is to be executed in the background */
	size_t argc;	/* argument count excluding the input/output
					   redirection symbols and the corresponding
					   filenames and the eventual '&' */
}Command;


/* Allocate buffer for parsed commands */
Command** allocate_cmd_buffer();


/* Free the buffer allocated by allocate_cmd_buffer() */
void free_cmd_buffer(Command **cmds);


/*
	Print to standard output the command's arguments followed by,
	input redirection, output redirection and &.

	If the input parameter is NULL or the argument vector is empty,
	nothing will be printed.
*/
void print_cmd(const Command *cmd);


/*
	Print to standard output each Command in cmds separated by |.
	It uses print_cmd() to print each Command.

	If the input parameter is NULL nothing will be printed.
*/
void print_cmds(const Command **cmds, size_t cmdc);


/*
	Copies the commands from src_cmds to dst_cmds and returns dst_cmds.

	Make sure that the size each of the vectors, src_cmds and dst_cmds, is cmdc.
	NULL will be returned if either of the vectors is NULL.
*/
Command ** copy_cmds(Command **dst_cmds, const Command **src_cmds, size_t cmdc);


/*
	Parse the string in 'line', generate a Command vector and return the
	number of Commands or -1 if the cmds is NULL.

	Each of the substring in 'line' delimited by '|' constitutes as the
	raw string to be parsed to generate a Command. If a substring ends with '&',
	then it implies a background Command.

	Input and output redirections can be achieved by specifying < for input
	and > for output, each followed by a pathname, and they must appear at the
	end of a substring but before '&'; otherwise, they will be
	considered as part of the argument vector to be passed to execvp().
	Moreover, both < and > must surrounded by whitespaces.

	NB: The function modifies 'line'; hence, passing a non-modifiable string
		may result in a segmentation fault.
*/
int parse_cmd_line(char line[], Command** cmds);


/*
	Execute each of the Commands in the vector cmds.
	If the cmds is NULL or it is empty, return -1.

	If cmds contains more than one Command, then the Commands are considered
	piped, i.e., the output of cmds[i] is the input of cmds[i+1].

	Moreover, cmds[0] is executed by the calling process while the remaining
	Commands are passed to a child process which in turn calls execute() on them.

	On success, a call to this has similar effect as calling execvp() in that
	it replaces the calling process.
*/
int execute(Command** cmds, size_t cmdc);

#endif
