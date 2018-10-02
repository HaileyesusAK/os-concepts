#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "types.h"
#include "stack.h"

int main() {
	const size_t capacity = 10;
	Stack *s;
	Node n;

	/* verify invalid input parameters are checked */
	assert(pop(s, NULL) == -1);
	assert(pop(NULL, &n) == -1);
	assert(pop(NULL, NULL) == -1);
	assert(push(s, NULL) == -1);
	assert(push(NULL, &n) == -1);
	assert(push(NULL, NULL) == -1);

	s = create_stack(capacity);

	/* verify a stack is created */
	assert(s != NULL);

	/* verify elements can be pushed onto the stack */
	Node nodes[capacity];
	for(int i = 0; i < capacity; ++i) {
		nodes[i].val = i;
		assert(push(s, &nodes[i]) == 0);
	}

	/* verify push is not possible on a full stack */
	assert(push(s, &n) == 1);

	/* verify the elements are properly pushed */
	for(int i = capacity-1; i >= 0; --i) {
		assert(pop(s, &n) == 0);
		assert(n.val == i);
	}

	/* verify empty stack cannot be popped */
	assert(pop(s, &n) == 1);

	/* verify stack is properly destroyed */
	assert(destory_stack(&s) == 0);
	assert(s == NULL);

	return 0;
}
