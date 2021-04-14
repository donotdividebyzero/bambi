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

    Ast *parser_result = expr();
    if (parser_result->error != NULL) {
        fprintf(pipe->out, "%s", parser_result->error);
        free(parser_result->error);
        token_stream_shutdown();
        return;
    }

    Ast *result = interpret(pipe, parser_result);

    if (result->error != NULL) {
        fprintf(pipe->out, "%s", result->error);
        free(result->error);
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

    token_stream_shutdown();
}