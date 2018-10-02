#ifndef __STACK_H__
#define __STACK_H__

#include<stdlib.h>

#include "types.h"

typedef struct {
	Node*  buf;
	size_t capacity;
	int top;	// Index on the buffer where the next stack element will be pushed
}Stack;

/*
	Creates a stack with given capcity and returns it.
	If the stack can not be created, NULL will be returned.
*/

Stack* create_stack(size_t capacity);
/*
	Frees the memory associated with the stack and
	assigns a NULL value in the input parameter.

	Returns 0 on sucess or -1 on failure.
*/
int destory_stack(Stack** s);

/*
	Pops the top element from the stack and copies it to the address
	pointed to by the second parameter.

	Returns -1 for NULL input parameters, 1 if the stack is empty
	or 0 on success.
*/
int pop(Stack* s, Node* n);

/*
	Pushes the content of the node pointed to by the second parameter onto the stack.
	
	Returns -1 for NULL input parameters, 1 if the stack is full 
	or 0 on success.
*/
int push(Stack* s, const Node* n);

#endif
