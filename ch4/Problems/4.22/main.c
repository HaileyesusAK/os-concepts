#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#define NUM_THREADS 3

long avg_value;	/* average value of the number series */
long min_value;	/* the minimum of the number series */
long max_value;	/* the maximum of the number series */

/* function to calculate the average value */
void *worker_avg(void *param);

/* function to calculate the minimum value */
void *worker_min(void *param);

/* function to calculate the maximum value */
void *worker_max(void *param);

typedef void* (*Worker)(void*);

/* a struct to represent a thread */
struct thread_t {
	pthread_t tid;					/* thread id */
	pthread_attr_t attr;			/* thread attributes */
	Worker func;					/* a pointer to the worker function */
};

int main(int argc, char *argv[]) {
	if(argc < 2) {
		fprintf(stderr, "Usage: %s <number series>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	struct thread_t threads[NUM_THREADS];
	Worker workers[] = {worker_avg, worker_min, worker_max};

	/* initialize and launch the threads */
	for(int i = 0; i < NUM_THREADS; ++i) {
		pthread_attr_init(&threads[i].attr);
		threads[i].func = workers[i];

		if(pthread_create(&threads[i].tid, &threads[i].attr, threads[i].func, &argv[1]) < 0){
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
	}

	/* wait for thread completion */
	for(int i = 0; i < NUM_THREADS; ++i)
		pthread_join(threads[i].tid, NULL);

	/* print statistical values */
	printf("The average value is %ld\n", avg_value);
	printf("The minimum value is %ld\n", min_value);
	printf("The maximum value is %li\n", max_value);

	return 0;
}

void *worker_avg(void *param) {
	int n = 0;
	long sum = 0;
	for(char** arg = (char **)param; *arg != NULL; ++arg, ++n) {
		sum += atol(*arg);
	}

	if(n)
		avg_value = sum / n;
	else
		avg_value = 0;

	pthread_exit(NULL);
}

void *worker_min(void *param) {
	min_value = LONG_MAX;
	long n;
	for(char** arg = (char **)param; *arg != NULL; ++arg) {
		n = atol(*arg);
		if(n < min_value)
			min_value = n;
	}

	pthread_exit(NULL);
}

void *worker_max(void *param) {
	max_value = LONG_MIN;
	long n;
	for(char** arg = (char **)param; *arg != NULL; ++arg) {
		n = atol(*arg);
		if(n > max_value)
			max_value = n;
	}

	pthread_exit(NULL);
}
