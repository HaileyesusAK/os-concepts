#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUF_SIZE (1<<16)

int main(int argc, char* argv[]) {
	if(argc != 2) {
		fprintf(stderr, "Usage: %s <positive integer>\n", argv[0]);
		return EXIT_FAILURE;
	}

	/* read user input */
	long n;
	n = strtol(argv[1], NULL, 10);
	if(n <= 0) {
		fprintf(stderr, "Usage: %s <positive integer>\n", argv[0]);
		return EXIT_FAILURE;
	}

	char* shm_name = "/shm_collatz_buf";

	/* create a shared memory*/
	int fd = shm_open(shm_name, O_RDWR | O_CREAT, 0666);
	if(fd < 0) {
		perror("shm_open");
		return EXIT_FAILURE;
	}

	if(ftruncate(fd, BUF_SIZE) < 0) {
		perror("ftruncate");
		return EXIT_FAILURE;
	}

	char *buf = mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(!buf) {
		perror("mmap");
		return EXIT_FAILURE;
	}

	pid_t pid = fork();
	if(pid < 0) {
		perror("fork");
		return EXIT_FAILURE;
	}
	else if(pid == 0) {
		size_t str_len = 0;
		while(n > 1) {
			str_len += sprintf(buf + str_len, "%ld ", n);
			if(n & 0x1)
				n = 3*n + 1;
			else
				n >>= 1;
		}

		sprintf(buf + str_len, "%ld", n);
	}
	else
	{
		wait(NULL);
		printf("%s\n", buf);
		shm_unlink(shm_name);
	}

	return EXIT_SUCCESS;
}
