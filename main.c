
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define CMDLINE_MAX 512

void cmd_check(char* cmd){

    int word_ctr = 1;
    int token_ctr = 0;
    bool token_check;
    for(int i = 0; i < 15; i++){
        token_ctr++;
        if(cmd[i]==' '){
            word_ctr++;
            if(token_ctr >= 32){
                token_check == false;
            }
        }
    }
    /* Print command line if stdin is not provided by terminal */
    if (!isatty(STDIN_FILENO)) {
        printf("%s", cmd);
        fflush(stdout);
    }
    /* check if arguments exceed 16 */
    else if(word_ctr > 15){
        printf("Error: too many process arguments \n");
    }
     /* check if argument exceeds 32 */
    else if (!(token_check)){
        printf("Error: exceeded max token count \n");
    }

}

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
        cmd_check(cmd);

        /* Remove trailing newline from command line */
        nl = strchr(cmd, '\n');
        if (nl)
            *nl = '\0';

        /* Builtin command */
        if (!strcmp(cmd, "exit")) {
            fprintf(stderr, "Bye...\n");
            fprintf(stderr, "+ completed '%s' [%d]\n", cmd, 0);
            break;
        }

        /* Regular command */
        retval = system(cmd);
//         pid = fork();
//         if (pid == 0) {
//             /* Child */
//             retval = execlp(cmd, cmd, NULL);
//             perror("execv");
//             exit(1);
//         } else if (pid > 0) {
//             /* Parent */
//             int status;
//             waitpid(pid, &status, 0);
//             printf("Child returned %d\n",
//                    WEXITSTATUS(status));
//         } else {
//             perror("fork");
//             exit(1);
//         }

        fprintf(stderr, "+ completed '%s' [%d]\n", cmd, retval);
    }

    return EXIT_SUCCESS;
}
