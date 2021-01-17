#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
	pid_t pid;
	char *cmd = "/bin/echo";
	char *args[] = { cmd, "ECS150", NULL};

	pid = fork();
	if (pid == 0) {
		/* Child */
		execv(cmd, args);
		perror("execv");
		exit(1);
	} else if (pid > 0) {
		/* Parent */
		int status;
		waitpid(pid, &status, 0);
		printf("Child returned %d\n",
			   WEXITSTATUS(status));
	} else {
		perror("fork");
		exit(1);
	}

    return 0;
}
