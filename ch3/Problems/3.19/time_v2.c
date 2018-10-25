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

	int pipefd[2];
	size_t SIZE = sizeof (struct timeval);
	pid_t pid;
	int wstatus;

	/* Create the pipe */
	if(pipe(pipefd) < 0) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	/* Create a child process */
	pid = fork();
	if(pid < 0) {
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else if(pid == 0) { /* child process */
		/* close the read end of the pipe */
		close(pipefd[0]);

		struct timeval start_time;
		gettimeofday(&start_time, NULL);

		/* write the current time into the pipe */
		if(write(pipefd[1], &start_time, sizeof (struct timeval)) < 0) {
			perror("CHILD: write");
			exit(EXIT_FAILURE);
		}

		/* close the write end of the pipe */
		close(pipefd[1]);

		/* execute the command */
		if(execvp(argv[1], &argv[1]) < 0) {
			exit(EXIT_FAILURE);
		}
	}
	else { /* parent process */
		/* close the write end of the pipe */
		close(pipefd[1]);

		wait(&wstatus);
		if(wstatus) {
			fprintf(stderr, "Unable to execute command: %s\n", argv[1]);
			exit(EXIT_FAILURE);
		}
		else {
			struct timeval start_time, end_time;
			gettimeofday(&end_time, NULL);

			/* read the start time from the pipe */
			if(read(pipefd[0], &start_time, sizeof (struct timeval)) < 0) {
				perror("PARENT: write");
				exit(EXIT_FAILURE);
			}

			close(pipefd[0]);
			printf("Elapsed time: %fs\n", get_elapsed_time(&start_time, &end_time));
		}
	}

	exit(EXIT_SUCCESS);
}
