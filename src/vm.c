#include "vm.h"
#include <stdlib.h>
#include <string.h>

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

size_t commands_size = sizeof(commands) / sizeof(RuntimeCommand);

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
    for (size_t i = 0; i < commands_size; ++i) {
        if (strcmp(name, commands[i].name) == 0) {
            return &commands[i];
        }
    }

    return NULL;	
}

void runtime(int argc, char** argv)
{
    printf("Runtime...\n");
}

int main(int argc, char **argv)
{
     if (argc > 1) {
        for(int i=1; i < argc; i++) {
            RuntimeCommand *cmd = find_command(argv[i]);
            if (cmd != NULL) {
                cmd->run(argc, argv);
            } else {
                fprintf(stderr, "ERROR: Command `%s` does not exist\n", argv[i]);
                RuntimeCommand *cmd = find_command("help");
                cmd->run(argc, argv);
                exit(EXIT_FAILURE);
            }
        }
    }
    else {
        RuntimeCommand *cmd = find_command("runtime");
        cmd->run(argc, argv);
    }

    return 0;
}