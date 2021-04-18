#include "runtime.h"

void runtime(int argc, char** argv, RuntimePipe *pipe)
{
    (void)argc;
    (void)argv;
    Tokenizer_Error tokenizer_result = tokenize(pipe->in);
    if (tokenizer_result.error != NULL) {
        fprintf(pipe->out, "%s", tokenizer_result.error);
        free(tokenizer_result.error);
        token_stream_shutdown();
        return;
    }

    Ast *program_result = program();

    if (program_result->error != NULL) {
        fprintf(pipe->out, "%s", program_result->error);
        free_ast(program_result);
        token_stream_shutdown();
        return;
    }

    create_global_context();
    Ast *result = interpret(pipe, get_global_context(), program_result);
    free_ast(program_result);
    free_global_context();

    if (result->error != NULL) {
        fprintf(pipe->out, "%s", result->error);
        free_ast(result);
        token_stream_shutdown();
        return;
    }

    if (result->type == AT_VALUE) {
        if (result->value->type == AV_NUMERIC) {
            if (result->value->numeric->type == AN_FLOAT) {
                fprintf(pipe->out, "%.8f", result->value->numeric->f);
            } else {
                fprintf(pipe->out, "%ld", result->value->numeric->l);
            }
        }
        if (result->value->type == AV_STRING) {
            fprintf(pipe->out, "%s", result->value->str->value);
        }
    } else {
        fprintf(pipe->out, "Wrong value returned, expected 'AT_VALUE' got: '%s'", ast_type_to_str(result->type));
    }

    free_ast(result);
    token_stream_shutdown();
}