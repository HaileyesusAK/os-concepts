#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>

/* test weather an integer greater than 2 is a prime number or not */
int is_prime(int n) {
	int i = (n == 2);
	if(n > 2){
		int m = sqrt(n);
		int j = 2;
		do {
			i = n % j;
			++j;
		}while(j <= m && i);
	}

	return i;
}
void* print_primes(void* param) {
	int n = *(int*)param;
	if(n >= 2)
		printf("2 ");

	for(int i = 3; i <= n; i += 2) {
		if(is_prime(i))
			printf("%d ", i);
	}
	printf("\n");

	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	if(argc < 2) {
		fprintf(stderr, "Usage: %s <number>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int n = atoi(argv[1]);

	pthread_t tid;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	if(pthread_create(&tid, &attr, print_primes, &n) < 0){
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	/* wait for thread completion */
	pthread_join(tid, NULL);

	return 0;
}
