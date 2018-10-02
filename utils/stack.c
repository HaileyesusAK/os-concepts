#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#include "types.h"
#include "stack.h"

Stack* create_stack(size_t capacity) {
	Stack *s = (Stack *)malloc(sizeof(Stack));
	if(!s) {
		perror("malloc: allocating stack");
		return NULL;
	}
	s->capacity = capacity;
	s->buf = (Node*)malloc(capacity * sizeof(Node));
	if(!s->buf) {
		perror("malloc: allocating stack buffer");
		free(s);
		return NULL;
	}

	s->top = 0;

	return s;
}

int destory_stack(Stack** s_ptr) {
	if(!s_ptr || !*s_ptr)
		return -1;

	free((*s_ptr)->buf);
	free(*s_ptr);
	*s_ptr = NULL;

	return 0;
}

int pop(Stack* s, Node *n) {
	if(!s || !n)
		return -1;

	if(s->top == 0)
		return 1;

	memcpy(n, s->buf + s->top - 1, sizeof(Node));
	s->top--;

	return 0;
}

int push(Stack* s, const Node *n) {
	if(!s || !n)
		return -1;

	if(s->top == s->capacity)
		return 1;

	memcpy(s->buf + s->top, n, sizeof(Node));
	s->top++;

	return 0;
}
