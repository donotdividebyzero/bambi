#ifndef __VM_H
#define __VM_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NULL
    #define NULL (void *)0
#endif

typedef struct VM VM;

typedef struct RuntimePipe {
    FILE *in;
    FILE *out;
    FILE *err;
} RuntimePipe;

typedef struct RuntimeCommand
{
    const char *name;
    const char *description;
    void (*run)(int argc, char **argv, RuntimePipe *);
} RuntimeCommand;

RuntimeCommand *find_command(const char *);

RuntimeCommand *get_commands();
size_t get_commands_size();
#endif
