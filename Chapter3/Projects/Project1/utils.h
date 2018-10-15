
#ifndef __OSH_H__
#define __OSH_H__

/*
	Allocates an array of 'argc' memory blocks, where each block is
	'argl' bytes long. Return 0 on success, -1 when 'args' is NULL
	or -2 on allocation failure.

	Make sure 'args' has at least 'argc' elements!
*/
int allocate_cmd_buffer(char *args[], size_t argc, size_t argl);

/*
	Frees the memory blocks allocated by allocate_cmd_buffer().
	Assigns NULL to each freed pointer. Return 0 on success or
	-1 when 'args' is NULL.

	Make sure 'args' has at least 'argc' elements!
*/
int free_cmd_buffer(char *args[], size_t argc);

/*
	Splits 'line' by whitespace into atmost 'max_count' 'words'.
	'words' is an array of 'max_count' char pointers to which
	the words are copied.

	Return the number of words on success, or -1 on failure.

	Make sure 'words' can hold at least 'max_count' elements!
*/

size_t get_words(char line[], char * words[], size_t max_count);

/*
	Copies 'argc' strings from 'src_args' to 'dst_args' and returns
	0 on success, -1 on failure (when 'words' is NULL).

	Make sure both 'dst_args' and 'src_args' have at least 'argc' elements!
*/
int copy_cmd(char *dst_args[], char * const src_args[], size_t argc);

/*
	Prints the strings in cmd separated by whitespace to stdout,
	returns 0 on success, -1 on failure (when 'cmd' is NULL).

	Make sure 'cmd' has at least 'argc' elements!

*/
int print_cmd(char *const cmd[], size_t argc);

#endif
