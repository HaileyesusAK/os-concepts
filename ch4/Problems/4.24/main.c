#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

long pts_in_circle; // number of points falling inside the circle

/*
	generate random points inside the circumscribing square
	and count those that fall inside the circle
*/
void* generate_rnd_pts(void* param) {
	long num_pts  = *(long*)param;

	/* slope of the line that maps numbers from [0, RAND_MAX] to [0, sqrt(2)] */
	double m = sqrt(2) / RAND_MAX;

	srand(time(NULL));
	for(long i = 0; i < num_pts; ++i) {
		if(m * rand() <= 1.0)
			++pts_in_circle;
	}

	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	if(argc != 2) {
		fprintf(stderr, "Usage: %s <number of points>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	long n = atol(argv[1]);
	if(n <= 0) {
		fprintf(stderr, "Usage: %s <number of points>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	pthread_t tid;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	if(pthread_create(&tid, &attr, generate_rnd_pts, &n) < 0){
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}

	pthread_join(tid, NULL);

	printf("%li %li %.3f\n", pts_in_circle, n, 1.0*pts_in_circle/n);
	double pie = 4.0 * pts_in_circle / n;
	printf("%0.3f\n", pie);
	return 0;
}
