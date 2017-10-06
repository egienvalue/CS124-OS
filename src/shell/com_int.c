/*
 * The Breakfast Club, CS 124, Fall 2017
 * Vaibhav Anand, Nikhil Gupta, Michael Hashe
 *
 * This file contains a method to handle all shell built in commands. It
 * currently handles exit, cd, chdir, and history.
 */

#include <unistd.h> // read, write
#include <stdio.h> // printf
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h> // pid_t, wait
#include <string.h>

#include "com_parser.h"
#include "mysh.h"
#include "err_hand.h"


int internal_command_handler(struct command *cmd, int only_command) {
    char *cmd_name = cmd->exec_fn;

    // switch statement in c won't work well
    if (strcmp(cmd_name, "exit") == 0) {
        if (only_command) {
            exit(0);
        }
    } else if ((strcmp(cmd_name, "cd") == 0) || 
               (strcmp(cmd_name, "chdir") == 0)) {
        int err_val;
        if ((cmd->argv[1] == NULL) || (strcmp(cmd->argv[1], "~") == 0)) {
            err_val = chdir(getenv("HOME"));
        } else {
            err_val = chdir(cmd->argv[1]);
        }

        // handle if an error is returned
        if (err_val == -1) {
            print_err();
        }

    } else if (strcmp(cmd_name, "history") == 0) {
        // TODO: maybe, in the future, allow piping the results of this into the
        // next cmd
        print_history();
    } else {
        return 0;
    }
    return 1;
}
