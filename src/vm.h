#ifndef __VM_H
#define __VM_H

    #include "help.h"
    #include "tests.h"

    #include <stdio.h>
    #include <stdlib.h>

    #ifndef NULL
        #define NULL (void *)0
    #endif

    typedef struct RuntimeCommand {
        const char *name;
        const char *description;
        void (*run)(int, char **);
    } RuntimeCommand;

    RuntimeCommand *find_command(const char *);

    RuntimeCommand *get_commands();
    size_t get_commands_size();

    typedef struct VM VM;

    void runtime(int, char**);
#endif
