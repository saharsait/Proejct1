
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define CMDLINE_MAX 512

int main(void)
{
    pid_t pid;

    char cmd[CMDLINE_MAX];
    char *args[] = { cmd, "ECS150", NULL};
    pid = fork();

    while (1) {

        char *nl;
        int retval;

        /* Print prompt */
        printf("sshell@ucd$ ");
        fflush(stdout);

        /* Get command line */
        fgets(cmd, CMDLINE_MAX, stdin);

        /* Print command line if stdin is not provided by terminal */
        if (!isatty(STDIN_FILENO)) {
            printf("%s", cmd);
            fflush(stdout);
        }

        /* Remove trailing newline from command line */
        nl = strchr(cmd, '\n');
        if (nl)
            *nl = '\0';

        /* Builtin command */
        if (!strcmp(cmd, "exit")) {
            fprintf(stderr, "Bye...\n");
            break;
        }

        /* Regular command */
        //retval = system(cmd);
        pid = fork();
        if (pid == 0) {
            /* Child */
            retval = execlp(cmd, cmd, NULL);
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

        fprintf(stderr, "+ completed '%s' [%d]\n", cmd, retval);
    }

    return EXIT_SUCCESS;
}
