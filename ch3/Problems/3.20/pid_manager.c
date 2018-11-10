/*
	The current implementation of pid manager uses a bitmap
	to represent the availability of process identifiers.

	It uses an array of 64-bit unsigned integers as a bitmap where
	the i-th element represents process identifiers in the range
	[MIN_PID + i * 64, MIN_PID + (i + 1) * 64 - 1]. The array is
	subdivided in blocks of 64 array elements and each element is
	refered to as a partition.

	The blocks are represented by an array of 64-bit unsigned integers
	where the i-th element is used as a bitmap to represent the
	availability the partitions under it. Similarly, the availability
	of the blocks are represented by a 64-bit unsigned integer.

	This approach uniquely identifies upto 64^3 (262144) processes.
*/

#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#include "pid_manager.h"

#define SET_BIT(n, i) (n |= (1llu<<i))
#define RESET_BIT(n, i) (n &= ~(1llu<<i))
#define IS_ALL_SET(n) (((~0) ^ n) == 0)
#define MIN(a, b) (((a) < (b))? (a) : (b))
#define MAX(a, b) (((a) > (b))? (a) : (b))

/* Get index of the least significant bit set to one */
static char get_lsb_64(uint64_t a) {
	char n = -1;
	if(a) {
		uint64_t b = a - 1;
		uint64_t c = a | b;
		c ^= b;
		n = log2(c);
	}

	return n;
}

/* Check if all the bits are set to one */
/*static inline char is_all_set(uint64_t n) {
	return (((~0) ^ n) == 0);
}
*/
static uint64_t* partitions;
static uint64_t* partition_masks;
static uint64_t blk_mask;
static int blk_cnt;

int allocate_map(void) {
	int partition_cnt = (PROC_COUNT + WIDTH - 1) / WIDTH;
	partitions = (uint64_t *)calloc(partition_cnt, sizeof (uint64_t));
	if(!partitions)
		return -1;

	blk_cnt = MIN(WIDTH, (partition_cnt + WIDTH - 1) / WIDTH);		// atmost WIDTH blocks

	partition_masks = (uint64_t *)calloc(blk_cnt, sizeof (uint64_t));
	if(!partition_masks)
		return -1;

	return 0;
}

int allocate_pid(void) {
	int pid = -1;
	char blk_id = get_lsb_64(~blk_mask);

	if(blk_id >= 0) {
		char partition_id = get_lsb_64(~partition_masks[blk_id]);	//partition id within the block
		uint64_t* partition = &partitions[blk_id * WIDTH + partition_id];
		char cell_id = get_lsb_64(~(*partition));

		pid = MIN_PID + blk_id * WIDTH * WIDTH + partition_id * WIDTH + cell_id;

        SET_BIT(*partition, cell_id);
		if(IS_ALL_SET(*partition)) {	                            // if the partition is exhausted
		    SET_BIT(partition_masks[blk_id], partition_id);         // mark the partition as used

			if(IS_ALL_SET(partition_masks[blk_id])) {               // if the block is exhausted
			    SET_BIT(blk_mask, blk_id);
			}
		}
	}

	return pid;
}

void release_pid(int pid) {
	if(pid >= MIN_PID && pid <= MAX_PID) {
		int i = pid - MIN_PID;

		int partition_id = i / WIDTH;	                            // The global partition id
		int cell_id = i % WIDTH;
		int blk_id = partition_id / WIDTH;
		int partition_local_id = partition_id % WIDTH;	            // the partition's id within the block
		uint64_t* partition = &partitions[partition_id];

        RESET_BIT(*partition, cell_id);                             // mark the cell as free
	    RESET_BIT(partition_masks[blk_id], partition_local_id);     // mark the partition as NOT exhausted
		RESET_BIT(blk_mask, blk_id);	                            // mark the block as NOT exhausted
	}
}
