#include "vm.h"

void help(int argc, char **argv, RuntimePipe *pipe)
{
    (void)argc;
    (void)argv;
    FILE *stream = pipe->err;
    int longest = 0;

    RuntimeCommand *commands = get_commands();

    for (size_t i = 0; i < get_commands_size(); ++i) {
        int n = strlen(commands[i].name);
        if (n > longest) {
            longest = n;
        }
    }

    fprintf(stream, "Usage:\n");
    fprintf(stream, "  Available subcommands:\n");
    for (size_t i = 0; i < get_commands_size(); ++i) {
        fprintf(stream, "    ./brumbrum %s%*s - %s\n",
                commands[i].name,
                (int) (longest - strlen(commands[i].name)), "",
                commands[i].description);
    }
}

RuntimeCommand commands[] = {
    {
        .name = "help",
        .description = "Usage, description",
        .run = help
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