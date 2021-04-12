#include <string.h>
#include <stdio.h>
#include "vm.h"
#include "tests.h"
#include "help.h"
#include "runtime.h"

RuntimeCommand commands[] = {
    {
        .name = "help",
        .description = "Usage, description",
        .run = help
    },
    {
        .name = "tests",
        .description = "Command that build and run test suite",
        .run = testsuite
    },
    {
        .name = "runtime",
        .description = "VM runtime, default command executed",
        .run = runtime
    }
};

RuntimeCommand *get_commands()
{
    return commands;
}

size_t get_commands_size()
{
    return sizeof(commands) / sizeof(RuntimeCommand);
}

RuntimeCommand *find_command(const char *name)
{
    for (size_t i = 0; i < get_commands_size(); ++i) {
        if (strcmp(name, commands[i].name) == 0) {
            return &commands[i];
        }
    }

    return NULL;
}

int main(int argc, char **argv)
{
    RuntimePipe pipe;
    pipe.in = stdin;
    pipe.out = stdout;
    pipe.err = stderr;
    
    if (argc > 1) {
        for(int i=1; i < argc; i++) {
            RuntimeCommand *cmd = find_command(argv[i]);
            if (cmd != NULL) {
                cmd->run(argc, argv, &pipe);
            } else {
                fprintf(pipe.err, "ERROR: Command `%s` does not exist\n", argv[i]);
                RuntimeCommand *cmd = find_command("help");
                cmd->run(argc, argv, &pipe);
                exit(EXIT_FAILURE);
            }
        }
    }
    else {
        RuntimeCommand *cmd = find_command("runtime");
        cmd->run(argc, argv, &pipe);
    }

    return 0;
}