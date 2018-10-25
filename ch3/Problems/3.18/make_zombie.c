#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define INACTIVE_TIME_S 10 // The parent process's inactive time in seconds

int main() {
	pid_t pid;

	pid = fork();
	if(pid < 0)
		perror("fork");
	else if(pid > 0) { /* parent process */
		sleep(INACTIVE_TIME_S + 1);
		wait(NULL);
	}

	return 0;	/* the child process terminates immediately */
}

