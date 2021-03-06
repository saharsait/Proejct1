#include <ctype.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>

#define ARGV_MAX 16
#define CMDLINE_MAX 512
#define TOKEN_MAX 32

struct CMD_LINE {
    char *argv[ARGV_MAX];
    int redirection;
    char *redirection_file;
    int piping;

};

void redirection_output(struct CMD_LINE CMD) {
    int fd;
    fd = open(CMD.redirection_file, O_WRONLY | O_CREAT | O_APPEND, 0600);
    dup2(fd, STDOUT_FILENO);
    close(fd);
}

struct CMD_LINE redirection_check(struct CMD_LINE CMD, char *cmd) {
    CMD.redirection = 0;
    int cmd_index;
    int space_ctr = 0;
    bool cmd_exist_after;
    char *piping_sign;
    char *redirection_sign;

    redirection_sign = strstr(cmd, ">");
    piping_sign = strstr(cmd, "|");

    for (unsigned int i = 0; i <= strlen(cmd); i++) {

        if (cmd[i] == '>') {
            CMD.redirection = 1;
            cmd[i] = ' ';
            cmd_index = i;
            break;
        } else if (isspace(cmd[i])) { //count how many spaces there were before >
            space_ctr++;
        }
    }
    if (CMD.redirection == 1) {
        if (space_ctr == cmd_index) { //is there a command before >?
            fprintf(stderr, "Error: missing command\n");
            fflush(stdout);
        }
        for (unsigned int i = cmd_index + 1; i < strlen(cmd); i++) {
            if (!isspace(cmd[i])) {
                cmd_exist_after = true;
            }
        }
        if (!cmd_exist_after || cmd[cmd_index + 1] == '\0') {
            fprintf(stderr, "Error: no output file \n");
            fflush(stdout);
        }
        if (redirection_sign < piping_sign) {
            fprintf(stderr, "Error: mislocated output redirection \n");
            fflush(stdout);
        }
    }
    return CMD;
}
//check how many pipes exist and check if they were implemented correctly
struct CMD_LINE piping_check(struct CMD_LINE CMD, char *cmd) {
    CMD.piping = 0;
    int first_char_index = 0;

    for (unsigned int i = 0; i <= strlen(cmd); i++) {
        if (cmd[i] == '|') {
            CMD.piping++;
        }
    }
    for (unsigned int i = 0; i < strlen(cmd) - 1; i++) {
        if (cmd[i] != ' ') {
            first_char_index = i;
            break;
        }
    }
    if (cmd[first_char_index] == '>' || cmd[first_char_index] == '|') {
        fprintf(stderr, "Error: missing command\n");
        fflush(stdout);
    }
    return CMD;
}

struct CMD_LINE parsing(struct CMD_LINE CMD, char *cmd) {
    char *cmd_token;
    cmd_token = strtok(cmd, " ");
    int argv_ctr = 0;

    while (cmd_token != NULL) {
        CMD.argv[argv_ctr] = cmd_token;
        if (strlen(cmd_token) > TOKEN_MAX) {
            fprintf(stderr, "Error: exceeded max token count\n");
            fflush(stdout);
        }
        cmd_token = strtok(NULL, " ");
        argv_ctr++;
    }
    if (argv_ctr > ARGV_MAX) {
        fprintf(stderr, "Error: too many process arguments\n");
    }
    if (CMD.redirection == 1) {
        CMD.redirection_file = CMD.argv[argv_ctr - 1];
    }
    //set the last arguement = to NULL for the execvp function
    CMD.argv[argv_ctr] = NULL;

    return CMD;
}

// This code was inspired from syscalls lecture 'slide 37'
void execution_piping(struct CMD_LINE *CMD) {
    int cmd_pipe[2];
    int status[CMD->piping - 1];
    pid_t pid[CMD->piping - 1];

    pipe(cmd_pipe);
    for (int i = 0; i < CMD->piping - 1; i++) {
        pid[i] = fork();
        if (pid[i] == 0) { //child
            dup2(cmd_pipe[1], STDOUT_FILENO);
            close(cmd_pipe[0]);
            close(cmd_pipe[1]);
            execvp(CMD[i].argv[0], CMD[i].argv);
            exit(1);
        }
        dup2(cmd_pipe[0], STDIN_FILENO);
        close(cmd_pipe[1]);
        close(cmd_pipe[0]);
    }
    //parent
    dup2(cmd_pipe[0], STDIN_FILENO);
    close(cmd_pipe[1]);
    close(cmd_pipe[0]);
    execvp(CMD[CMD->piping - 1].argv[0], CMD[CMD->piping - 1].argv);
    for (int i = 0; i < CMD->piping - 1; i++) {
        waitpid(pid[i], &status[i], 0); // getting the status of each executed child
    }
}

// This code was inspired from syscalls lecture 'slide 20'
int execution(struct CMD_LINE CMD) {
    pid_t pid;
    int status;
    int stats;

    pid = fork();
    if (pid == 0) {
        //Child
        if (CMD.redirection == 1) {
            CMD.redirection = 0;
            redirection_output(CMD);
        }
        execvp(CMD.argv[0], CMD.argv);// might use execv
        fprintf(stderr, "Error: command not found\n");
        fflush(stdout);
        perror("execvp");
        exit(1);
    } else if (pid > 0) {
        //Parent
        //Waiting for the cloned process 'the child' to until it finish execution
        waitpid(pid, &status, 0);
        stats = WEXITSTATUS(status);
    } else {
        //In case of failing
        perror("error with fork");
        exit(1);
    }

    return stats;
}


int main(void) {
    char cmd[CMDLINE_MAX];
    char cmd_unchanged[CMDLINE_MAX];

    while (1) {
        char *nl;
        int retval;

        /* Print prompt */
        printf("sshell@ucd$ ");
        fflush(stdout);

        /* Get command line and check if it's less than the maximum*/
        if (!fgets(cmd, CMDLINE_MAX, stdin)) {
            break;
        };
        strcpy(cmd_unchanged, cmd);
        /* Print command line if stdin is not provided by terminal */
        if (!isatty(STDIN_FILENO)) {
            printf("%s", cmd);
            fflush(stdout);
        }
        /* Remove trailing newline from command line */
        nl = strchr(cmd, '\n');
        if (nl)
            *nl = '\0';
        nl = strchr(cmd_unchanged, '\n');
        if (nl)
            *nl = '\0';

        //Before determing the type of the command line, we need to parse
        // the command line first, then check if it's valid and follows all
        // the specified rules in the prompt
        struct CMD_LINE CMD;
        CMD = piping_check(CMD, cmd);
        CMD = redirection_check(CMD, cmd);
        if (CMD.piping == 0) {             // without piping
            struct CMD_LINE cmd_parsed = parsing(CMD, cmd);
            /* Builtin command */
            if (!strcmp(cmd, "exit")) {
                fprintf(stderr, "Bye...\n");
                fprintf(stderr, "+ completed '%s' [%d]\n",
                        cmd_unchanged, 0);
                break;
            }
                //https://iq.opengenus.org/chdir-fchdir-getcwd-in-c/ for getcwd
            else if (!strcmp(cmd, "pwd")) {
                char *current_path;
                char *buffer = NULL;
                current_path = getcwd(buffer, 0);
                //Since we're assuming the builtin commands will
                // always be correct,no need to check for errors
                fprintf(stderr, "%s\n", current_path);
            }
                //https://iq.opengenus.org/chdir-fchdir-getcwd-in-c/ for chdir
            else if (!strcmp(cmd, "cd")) {
                int new_dir = chdir(cmd_parsed.argv[1]);
                if (new_dir == -1) {
                    fprintf(stderr, "Error: cannot cd into directory");
                    fprintf(stderr, "+ completed '%s' [%d]\n", cmd_unchanged, 0);
                    break;
                }
            }
            else {
                /* Regular command */
                retval = execution(cmd_parsed);
            }

        }


        fprintf(stderr, "+ completed '%s' [%d]\n", cmd_unchanged, retval);
    }
    return EXIT_SUCCESS;
}
