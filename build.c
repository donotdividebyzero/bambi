#include "src/thread.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>

int main(int argc, char **argv)
{
    DIR *fd;
    struct dirent *in_file;

    if (NULL == (fd = opendir ("./src")))
    {
        fprintf(stderr, "Error : Failed to open directory './src' - %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    char **args;
    args = malloc(sizeof(char*) * 3);
    args[0] = "gcc";
    args[1] = "-o";
    args[2] = "brumbrum";

    int file_count = 0;

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
        int pos = (2 + file_count);

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

    RuntimeThread thread = {
        .args = args
    };
   
    wait_for_pid(run_async(&thread, NULL, NULL));
    return 0;
}
