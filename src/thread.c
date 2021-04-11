#include "./thread.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void wait_for_pid(int pid)
{
    for (;;) {
        int wstatus = 0;
        if (waitpid(pid, &wstatus, 0) < 0) {
            fprintf(stderr, "Can't wait for pid %d", pid);
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(wstatus)) {
            int exit_status = WEXITSTATUS(wstatus);
            if (exit_status != 0) {
                fprintf(stderr, "command exited with exit code %d\n", exit_status);
                exit(EXIT_FAILURE);
            }

            break;
        }

        if (WIFSIGNALED(wstatus)) {
            fprintf(stderr, "command process was terminated by %s", strsignal(WTERMSIG(wstatus)));
            exit(EXIT_FAILURE);
        }
    }
}

int run_async(RuntimeThread *command, int *in, int *out)
{
        int pid = fork();
        char *program = command->args[0];
        if (pid < 0) {
            fprintf(stderr, "Can't create execution thread for command %s", program);
            exit(EXIT_FAILURE);
        }

         if (pid == 0) {
            if (in) {
                if (dup2(*in, STDIN_FILENO) < 0) {
                    fprintf(stderr, "Can't setup stdin for child command :%s", program);
                    exit(EXIT_FAILURE);
                }
            }

            if (out) {
                if (dup2(*out, STDOUT_FILENO) < 0) {
                    fprintf(stderr, "Can't setup stdout for child command :%s", program);
                    exit(EXIT_FAILURE);
                }
            }

            if (execvp(program, (char * const*) command->args) < 0) {
                fprintf(stderr, "Can't exec child command: %s\n", program);
                exit(EXIT_FAILURE);
            }
        }

    return pid;
}
