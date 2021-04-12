#include "token.h"
#include "runtime.h"

#include <stdio.h>

void runtime(int argc, char** argv, RuntimePipe *pipe)
{
    
    Token *token_list = token_stream_tokenize(pipe->in);
}