#include "vm.h"
#include "help.h"
#include <stdio.h>
#include <string.h>

void help(int argc, char **argv, RuntimePipe *pipe)
{
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