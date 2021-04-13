#include "token.h"
#include "runtime.h"

#include <stdio.h>
#include "ast.h"
#include "interpreter.h"

void runtime(int argc, char** argv, RuntimePipe *pipe)
{
    (void)argc;
    (void)argv;
    Tokenizer_Error result = tokenize(pipe->in);
    if (result.error != NULL) {
        fprintf(pipe->out, "%s", result.error);
        free(result.error);
        token_stream_shutdown();
        return;
    }

    Ast expr_expr = expr();
    if (expr_expr.error != NULL) {
        fprintf(pipe->out, "%s", expr_expr.error);
        free(expr_expr.error);
        token_stream_shutdown();
        return;
    }

    fprintf(pipe->out, "%ld", expr_expr.number.l);
    token_stream_shutdown();
}