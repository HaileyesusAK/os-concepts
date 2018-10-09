#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BUF_SIZE (1<<16)

int main(int argc, char* argv[]) {
	if(argc != 3) {
		fprintf(stderr, "Usage: %s <src file name> <dest file name>\n", argv[0]);
		return EXIT_FAILURE;
	}

	int pipefd[2];
	if(pipe(pipefd) < 0) {
		perror("pipe");
		return EXIT_FAILURE;
	}
	
	char buf[BUF_SIZE];

	pid_t pid = fork();
	if(pid < 0) {
		perror("fork");
		return EXIT_FAILURE;
	}
	else if(pid == 0) { /* child process */
		close(pipefd[1]);	// close the write end

		/* open the destination file for writing */
		int fd = open(argv[2], O_WRONLY | O_CREAT, 0666);
		if(fd < 0) {
			perror("opening destination file");
			return EXIT_FAILURE;
		}

		/* read from the pipe */
		size_t count = read(pipefd[0], buf, BUF_SIZE);
		if(count < 0) {
			perror("reading from pipe");
			return EXIT_FAILURE;
		}

		if(write(fd, buf, count) < 0) {
			perror("writing to the destination file");
			return EXIT_FAILURE;
		}

		close(fd);
		close(pipefd[0]);
	}
	else { /* parent process */
		close(pipefd[0]);	// close the read end
		
		/* open the source file for reading */
		int fd = open(argv[1], O_RDONLY);
		if(fd < 0) {
			perror("opening source file");
			return EXIT_FAILURE;
		}

		/* read the source file */
		size_t count = read(fd, buf, BUF_SIZE);
		if(count < 0) {
			perror("reading from the source file");
			return EXIT_FAILURE;
		}

		/* write to pipe  */
		if(write(pipefd[1], buf, count) < 0) {
			perror("writing to pipe");
			return EXIT_FAILURE;
		}

		close(fd);
		close(pipefd[1]);
	}

	return EXIT_SUCCESS;
}
