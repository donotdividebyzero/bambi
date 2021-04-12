#include "token.h"
#include "runtime.h"

#include <stdio.h>

void runtime(int argc, char** argv, RuntimePipe *pipe)
{
    (void)argc;
    (void)argv;
    Token *token_list = token_stream_tokenize(pipe->in);

    while (token_list != NULL) {
        fprintf(pipe->out, "%s", token_list->value);
        token_list = token_list->next;
    }
}