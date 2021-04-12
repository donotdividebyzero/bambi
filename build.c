#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>

void wait_for(int pid) 
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

int run_async(const char **args)
{
    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Can't create execution thread for command %s", args[0]);
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        int *in;
        int *out;
        in = NULL;
        out = NULL;
        if (dup2(*in, STDIN_FILENO) < 0) {
            fprintf(stderr, "Can't setup stdin for child command :%s", args[0]);
            exit(EXIT_FAILURE);
        }

        if (dup2(*out, STDOUT_FILENO) < 0) {
            fprintf(stderr, "Can't setup stdout for child command :%s", args[0]);
            exit(EXIT_FAILURE);
        }

        if (execvp(args[0], (char * const*) args) < 0) {
            fprintf(stderr, "Can't exec child command: %s\n", args[0]);
            exit(EXIT_FAILURE);
        }
    }

    return pid;
}

int main(int argc, char **argv)
{
    DIR *fd;
    struct dirent *in_file;

    if (NULL == (fd = opendir ("./src")))
    {
        fprintf(stderr, "Error : Failed to open directory './src' - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    const char **args;
    args = malloc(sizeof(char*) * 3);
    args[0] = "gcc";
    args[1] = "-o";
    args[2] = "brumbrum";

    int file_count = 0;
    int pos = (2 + file_count);
    while ((in_file = readdir(fd)))
    {
        if (!strcmp (in_file->d_name, ".")) {
            continue;
        }

        if (!strcmp (in_file->d_name, "..")) {
            continue;
        }
        char *extension;
        if((extension = strrchr(in_file->d_name,'.')) != NULL ) {
            if(strcmp(extension,".c") != 0) {
                continue;
            }
        }

        ++file_count;
        
        pos = (2 + file_count);
        args = realloc(args, sizeof(char*) * (pos + 1));
        if (args == NULL) {
            fprintf(stderr, "Can't reallocate memory for brumbrum compilation\n");
            exit(EXIT_FAILURE);
        }

        char *file_name = malloc(strlen(in_file->d_name) + 6);
        strcat(file_name, "./src/");
        strcat(file_name, in_file->d_name);
        args[pos] = file_name;
    }

    wait_for(run_async(args));

    return 0;
}