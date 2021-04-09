#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define FLAGS "-Wall", "-Wextra", "-Wswitch-enum", "-Wmissing-prototypes", "-Wconversion", "-Wno-missing-braces", "-pedantic", "-fno-strict-aliasing", "-std=c11"

#ifndef NULL
    #define NULL (void *)0
#endif

typedef struct {
    const char *name;
    const char *description;
    void (*run)(int, char **);
} BuilderCommand;

typedef struct {
    const char *program;
    char **args;
} RuntimeCommand;

BuilderCommand *find_command(const char *);
void print_help(FILE *stream);

int run_async(RuntimeCommand *, int *, int *);
void wait_for_pid(int pid);

void help(int, char **);
void tests(int, char **);
void build_vm(int, char **);
void tools(int, char **);


// @TODO figure out how to spread char** as files
void compile_command(char **files, char *output_file)
{
    char *args[] = {
        "gcc",
        "-I", "./",
        FLAGS,
        "-o", output_file,
        NULL
    };

    RuntimeCommand compile = {
        .program = "gcc",
        .args = args
    };

    wait_for_pid(run_async(&compile, NULL, NULL));
}

BuilderCommand commands[] = {
    {
        .name = "help",
        .description = "Usage, description",
        .run = help
    },
    {
        .name = "tests",
        .description = "Command that build and run test suite",
        .run = tests
    },
    {
        .name = "vm",
        .description = "Build VL VM",
        .run = build_vm
    },
    {
        .name = "tools",
        .description = "Re-build vltool",
        .run = tools
    }
};

size_t commands_size = sizeof(commands) / sizeof(BuilderCommand);

int main(int argc, char **argv)
{
    char *command_name = "help";
    //@TODO: handle that we can build multiple commands
    //ex ./vltool build tests
    if (argc > 1) {
        for(int i=1; i < argc; i++) {
            BuilderCommand *command = find_command(argv[i]);
            if (command != NULL) {
                command->run(argc, argv);
            } else {
                fprintf(stderr, "ERROR: Command `%s` does not exist\n", argv[i]);
                print_help(stderr);
                exit(EXIT_FAILURE);
            }
        }
    }
    else {
        print_help(stdout);
    }

    return 0;
}


BuilderCommand *find_command(const char *name)
{
    for (size_t i = 0; i < commands_size; ++i) {
        if (strcmp(name, commands[i].name) == 0) {
            return &commands[i];
        }
    }

    return NULL;	
}

void print_help(FILE *stream)
{
    int longest = 0;

    for (size_t i = 0; i < commands_size; ++i) {
        int n = strlen(commands[i].name);
        if (n > longest) {
            longest = n;
        }
    }

    fprintf(stream, "Usage:\n");
    fprintf(stream, "  Available subcommands:\n");
    for (size_t i = 0; i < commands_size; ++i) {
        fprintf(stream, "    ./vltool %s%*s - %s\n",
                commands[i].name,
                (int) (longest - strlen(commands[i].name)), "",
                commands[i].description);
    }
}

void build_vm(int argc, char **argv)
{
    printf("Building VL VM\n");
}

void help(int argc, char **argv)
{
    print_help(stdout); 
}

void tests(int argc, char **argv)
{
    printf("Building and running test suite\n");
    char *args[] = {
        "gcc",
        "-I", "./include",
        "tests.c", 
        "-o", "test_runner", 
        NULL
    };
    RuntimeCommand compile_test_runner = {
        .program = "gcc",
        .args = args
    };

    wait_for_pid(run_async(&compile_test_runner, NULL, NULL));

    char *args1[] = {"./test_runner", "./tests", NULL};
    RuntimeCommand run_tests = {
        .program = "./test_runner",
        .args = args1
    };

    wait_for_pid(run_async(&run_tests, NULL, NULL));
}

void tools(int argc, char **argv)
{
    printf("Rebuilding vltools...\n\n\n");
    char *args[] = {"gcc", "build.c", "-o", "vltool", NULL};
    RuntimeCommand cmd = {
        .program = "gcc",
        .args = args
    };

    wait_for_pid(run_async(&cmd, NULL, NULL));
}

int run_async(RuntimeCommand *command, int *in, int *out)
{
        int pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Can't create execution thread for command %s", command->program);
            exit(EXIT_FAILURE);
        }

         if (pid == 0) {
            if (in) {
                if (dup2(*in, STDIN_FILENO) < 0) {
                    fprintf(stderr, "Can't setup stdin for child command :%s", command->program);
                    exit(EXIT_FAILURE);
                }
            }

            if (out) {
                if (dup2(*out, STDOUT_FILENO) < 0) {
                    fprintf(stderr, "Can't setup stdout for child command :%s", command->program);
                    exit(EXIT_FAILURE);
                }
            }

            if (execvp(command->program, (char * const*) command->args) < 0) {
                fprintf(stderr, "Can't exec child command: %s\n", command->program);
                exit(EXIT_FAILURE);
            }
        }

    return pid;
}

void wait_for_pid(int pid)
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
