#include <unistd.h> // read, write
#include <stdio.h> // cout, cin
#include <stdlib.h>
#include <assert.h>

#include "consts.h"
#include "com_parser.h"
#include "com_exec.h"

/* 
 * Handles forking off and executing a linked list of command that are piped 
 * together and executed in parrallel. It setups up redirection and pipes in 
 * the child processes (commands). It then waits for the processes to complete 
 * in the shell process.
 */

// TODO: How to figure out when the children processes have finished execution? Get this info


/* Executes the command after forking and does the io redirection to stdout, 
stdin, or stderr if needed. The io redirection is done after forking, so the 
redirection is unique to this process/command. */
void execute_command(struct command *cmd) {
    if (cmd->input_fn != NULL) {
        int fd0 = open(cmd->input_fn, O_RDONLY);
        dup2(fd0, STDIN_FILENO);
        close(fd0);
    }
    if (cmd->ouput_fn != NULL) {
        // creat() is equivalent to open(path, O_WRONLY|O_CREAT|O_TRUNC, mode)
        int fd1 = create(cmd->output_fn, 0644); // TODO: what mode to use? suggested online: 0644
        dup2(fd1, STDOUT_FILENO);
        close(fd1);
    }
    if (cmd->error_fn != NULL) {
        int fd2 = create(cmd->error_fn, 0644);
        dup2(fd2, STDERR_FILENO);
        close(fd2);
    }

    // Execute the command with its arguments
    execvp(command->exec_fn, command->argv);
    
    // The executable has control over the process now, or else:
    printf(STDERR_FILENO, "Failed to exec %s\n", cmd->exec_fn);
    exit(1);
}

/* Deprecated testing functions

void test_piping() {
    int n; // number of lines read/written from file
    int fd[2];
    pid_t pid; // pid of the child process
    char line[MAXLINE];

    if (pipe(fd) < 0)
        return -1; // pipe error
    if ((pid = fork()) < 0) {
        return -1; // fork error
    } else if (pid > 0) {
        // we are in the parent process
        close(fd[0]);
        write(fd[1], "hello world\n", 12);
    } else {
        // we are in the child preocess
        close(fd[1]);
        n = read(fd[0], line, MAXLINE);
        printf("number of bites read: %d\n", n);
        write(STDOUT_FILENO, line, n);
    }
}
*/

int main(int argc, char *argv[]) {
    printf("test:\n");
    // test_piping();

    return 0;
}





