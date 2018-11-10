#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "pid_manager.h"

/* Test the API*/
int main() {

	int status = allocate_map();
	assert(status == 0);

	int allocated_ids[PROC_COUNT];
	memset(allocated_ids, 0, PROC_COUNT * sizeof (int));

	int partition_cnt = (PROC_COUNT + WIDTH - 1) / WIDTH;

	// verify the ids are allocated in the given range and are unique
	for(int i = 0; i < PROC_COUNT; ++i) {
		int pid = allocate_pid();
		assert(pid >= MIN_PID && pid <= MAX_PID);
		assert(allocated_ids[pid - MIN_PID] == 0);

		// mark pid as allocated
		allocated_ids[pid - MIN_PID] = 1;
	}

	// release all the process ids
	for(int pid = MIN_PID; pid <= MAX_PID; ++pid) {
		release_pid(pid);
		// mark pid as release
		allocated_ids[pid - MIN_PID] = 0;
	}

	// verify all the ids can be reallocated once all are released
	for(int i = 0; i < PROC_COUNT; ++i) {
		int pid = allocate_pid();
		assert(pid >= MIN_PID && pid <= MAX_PID);
		assert(allocated_ids[pid - MIN_PID] == 0);

		// mark pid as allocated
		allocated_ids[pid - MIN_PID] = 1;
	}

	return 0;
}
