#include "vm.h"

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