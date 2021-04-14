#include "vm.h"

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