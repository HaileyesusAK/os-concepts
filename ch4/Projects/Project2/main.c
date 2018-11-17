#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <time.h>
#include "sort.h"

int* data;
int* sorted_data;

#define MAXINT 100 /* max element value in the test data */

int main(int argc, char* argv[]) {

	if(argc != 3) {
		fprintf(stderr, "Usage: %s <number of tests> <number of elements>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int test_count = atoi(argv[1]);
	int elem_count = atoi(argv[2]);

	if(test_count <= 0 || elem_count <= 0) {
		fprintf(stderr, "Usage: %s <number of tests> <number of elements>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* allocate memory for the input and output arrays */
	data = malloc(elem_count * sizeof(int));
	if(!data) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	
	sorted_data = malloc(elem_count * sizeof(int));
	if(!sorted_data) {
		perror("malloc");
		free(data);
		exit(EXIT_FAILURE);
	}

	int j;
	srand(time(NULL));	
	for(int i = 0; i < test_count; ++i) {

		/* Generate random test data */
		for(j = 0; j < elem_count; ++j)
			data[j] = rand() % (MAXINT + 1);

		/* sort the array */
		int err = sort(data, sorted_data, elem_count);
		if(err < 0) {
			fprintf(stderr, "sort failed: %d\n", err);
			free(data);
			free(sorted_data);
			exit(EXIT_FAILURE);
		}

		/* verify that the data is indeed sorted in ascending order */
		for(j = 1; j < elem_count; ++j)
			if(sorted_data[j] < sorted_data[j-1])
				break;
	
		/* print the sorted array */
		if(j != elem_count)
			printf("WRONG ORDER\n");
		else 
			printf("SORTED\n");
		
		for(j = 0; j < elem_count; ++j)
			printf("%d ", sorted_data[j]);
		printf("\n");
	}
	
	free(data);
	free(sorted_data);
	
	return 0;
}
