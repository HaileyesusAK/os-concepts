#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int* fib_sequence;	// a pointer to the fibonacci sequence

void* generate_sequence(void* param) {
	int n = *(int*)param;
	int f, f1 = 0, f2 = 1;

	for(int i = 0; i < n; ++i) {
		fib_sequence[i] = f1;
		f = f1 + f2;
		f1 = f2;
		f2 = f;
	}

	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	if(argc != 2) {
		fprintf(stderr, "Usage: %s <positive number>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int n = atoi(argv[1]);
	if(n <= 0) {
		fprintf(stderr, "Usage: %s <positive number>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* allocate memory for the fibonacci sequence */
	fib_sequence = malloc(n*sizeof(int));
	if(!fib_sequence) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	pthread_t tid;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	if(pthread_create(&tid, &attr, generate_sequence, &n) < 0){
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	pthread_join(tid, NULL);

	for(int i = 0; i < n; ++i)
		printf("%d ", fib_sequence[i]);

	printf("\n");

	free(fib_sequence);
	return 0;
}
