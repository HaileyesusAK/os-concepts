#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <wait.h>

/*
	Get elapsed time from the start time to the end time.
	A negative value of the elapsed time will be returned
	if the end time is earlier than the start time.
*/

double get_elapsed_time(struct timeval *start_time, struct timeval *end_time) {
	double elapsed_seconds = end_time->tv_sec - start_time->tv_sec;
	elapsed_seconds += (double)(end_time->tv_usec - start_time->tv_usec) / 1e6;
	return elapsed_seconds;
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		fprintf(stderr, "Usage: %s <command>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	int fd;
	struct timeval* start_time_ptr, end_time;
	const char* shm_name = "/shm_starttime";
	size_t SIZE = sizeof (struct timeval);
	pid_t pid;
	int wstatus;

	/* Open shared memory with read-and-write mode*/
	fd = shm_open(shm_name, O_RDWR | O_CREAT, 0666);
	if(fd < 0) {
		perror("shm_open");
		exit(EXIT_FAILURE);
	}

	/* Limit the shared memory size to the size of struct timeval*/
	if(ftruncate(fd, SIZE) < 0) {
		perror("ftruncate");
		shm_unlink(shm_name);
		exit(EXIT_FAILURE);
	}

	/* Create a mapping to the file*/
	start_time_ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(start_time_ptr == MAP_FAILED) {
		perror("mmap");
		shm_unlink(shm_name);
		exit(EXIT_FAILURE);
	}

	/* Create a child process */
	pid = fork();
	if(pid < 0) {
		perror("fork");
		shm_unlink(shm_name);
		exit(EXIT_FAILURE);
	}
	else if(pid == 0) { /* child process */
		gettimeofday(start_time_ptr, NULL);
		if(execvp(argv[1], &argv[1]) < 0) {
			exit(EXIT_FAILURE);
		}
	}
	else { /* parent process */
		wait(&wstatus);
		if(wstatus) {
			fprintf(stderr, "Unable to execute command: %s\n", argv[1]);
			shm_unlink(shm_name);
			exit(EXIT_FAILURE);
		}
		else {
			gettimeofday(&end_time, NULL);
			printf("Elapsed time: %fs\n", get_elapsed_time(start_time_ptr, &end_time));
		}
	}

	shm_unlink(shm_name);
	exit(EXIT_SUCCESS);
}
