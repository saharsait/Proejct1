
# Creating an executable
sshell: sshell.o
    gcc -Wall -Wextra -Werror -o sshell sshell.o

# Generate objects file from the C file
sshell.o: sshell.c
    gcc -Wall -Wextra -Werror -c -o sshell.o sshell.c

# Cleaning all the generated files
clean:
    rm -f sshell sshell.o



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CMDLINE_MAX 512

int main(void)
{
    char cmd[CMDLINE_MAX];

    while (1) {
        char *nl;
        int retval;

        /* Print prompt */
        printf("sshell$ ");
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
        retval = system(cmd);
        fprintf(stderr, "+ completed '%s' [%d]\n",
                cmd, retval);
    }

    return EXIT_SUCCESS;
}


// fork-exec-wait
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <sys/wait.h>
//int main(void){
//    pid_t pid;
//    char *cmd = "/bin/echo";
//    char *args[] = { cmd, "ECS150", NULL};
//    pid = fork();
//    if (pid == 0) {
//        /* Child */
//        execv(cmd, args);
//        perror("execv");
//        exit(1);
//    } else if (pid > 0) {
//        /* Parent */
//        int status;
//        waitpid(pid, &status, 0);
//        printf("Child returned %d\n",   WEXITSTATUS(status));
//    } else {
//        perror("fork");
//        exit(1);
//    }
//    return 0;
//}



//#include <stdio.h>
//#include <unistd.h>
//int a = 42;
//int main(int argc, char *argv[]){
//    int b = 23;
//    printf("Hello world!\n");
//    fork();
//    printf("My favorite number is %d.\n",   argc + a + b);
//    return 0;
//}


// dup2
//
//#include <fcntl.h>
//#include <stdio.h>
//#include <unistd.h>
//int main(void){
//    int fd;
//    printf("Hello #1\n");
//
//    fd = open("myfile.txt",  O_WRONLY | O_CREAT,  0644);
//
//    dup2(fd, STDOUT_FILENO);
//    close(fd);
//
//    printf("Hello #2\n");
//    return 0;
//}
