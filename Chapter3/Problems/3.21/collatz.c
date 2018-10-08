#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
	if(argc != 2) {
		fprintf(stderr, "Usage: %s <positive integer>\n", argv[0]);
		return EXIT_FAILURE;
	}

	long n = strtol(argv[1], NULL, 10);
	if(n <= 0) {
		fprintf(stderr, "Usage: %s <positive integer>\n", argv[0]);
		return EXIT_FAILURE;
	}

	pid_t pid = fork();
	if(pid < 0) {
		perror("fork");
		return EXIT_FAILURE;
	}
	else if(pid == 0) { /* child process */
			while(n > 1) {
				printf("%lu ", n);
				if(n & 0x1)
					n = 3*n + 1;
				else
					n >>= 1;
			}
			printf("%lu\n", n);
	}
	else	/* parent process */
	{
		wait(NULL);
	}


	return EXIT_SUCCESS;
}
