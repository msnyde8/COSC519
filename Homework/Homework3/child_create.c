#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
	printf("Parent PID: %d\n", getpid());
	pid_t pid;
	int childNum;

	for(childNum = 1; childNum < 5; ++childNum)
	{
		pid = fork();

		if (pid < 0) {
			fprintf(stderr, "Fork Failed");
			return 1;
		}
		else if (pid == 0) {
			printf("Child %d PID: %d\n", childNum, getpid());
			execlp("/home/mjs/Homework3/hello", "hello", NULL);
		}
		else {
			wait(NULL);
			printf("Child %d Complete\n", childNum);
		}
	}

	printf("All children complete\n");
	return 0;
}
