#include "vm.h"
#include "ast.h"
#include "interpreter.h"
#include "functions.h"
#include <stdio.h>
#include <string.h>

Ast *function_println(Context *ctx, Ast *arg)
{
    Ast *result = function_print(ctx, arg);
    printf("\n");
    return result;
}

// this function do not return anything
Ast *function_print(Context *ctx, Ast *args)
{
    if (args->type == AT_EMPTY) {
        print_compiler_error(args->token, "No arguments passed to function!");
        exit(EXIT_FAILURE);
    }

    Ast *arg = args->expr_list.exprs;
   
    while (arg) {
        Ast *ast = interpret(ctx, arg);
        Value *value = &ast->value;
        if (value->type == AV_BOOL) {
            if (value->b) {
                printf("%s", "true");
            } else {
                printf("%s", "false");
            }
        }
        if (value->type == AV_STRING) {
            printf("%.*s", (int)value->str->size, value->str->text);
        }
        else if (value->type == AV_NUMBER) {
            printf("%.6f", value->n);
        }
        arg = arg->next;
    }

    return make_empty(args->token);
}
