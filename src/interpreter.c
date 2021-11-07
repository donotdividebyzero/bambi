#include "vm.h"
#include "ast.h"
#include "context.h"
#include "functions.h"
#include "interpreter.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

Function *functions;

Function builtin_functions[] = {
    {.type = F_BUILTIN,
     .name = {5, "print"},
     .bfunc = {.fp = function_print, .argc = -1}},
    {.type = F_BUILTIN,
     .name = {7, "println"},
     .bfunc = {.fp = function_println, .argc = -1}}};

Function *find_function(const String *fnName)
{
    Function *candidate = functions;
    while (candidate)
    {
        if (string_equal(&candidate->name, fnName))
            return candidate;
        candidate = candidate->next;
    }

    size_t builtin_size = sizeof(builtin_functions) / sizeof(Function);
    for (size_t i = 0; i < builtin_size; i++)
    {
        if (string_equal(&builtin_functions[i].name, fnName))
            return &builtin_functions[i];
    }

    return NULL;
}

void add_function(Function *new_function)
{
    Function *function = functions;
    new_function->next = NULL;

    if (!function)
    {
        functions = new_function;
    }
    else
    {
        while (function->next)
            function = function->next;
        function->next = new_function;
    }
}

Ast *interpret_if(Context *ctx, Ast *ast)
{
    If _if = ast->_if;
    Ast *condition = interpret(ctx, _if.condition);
    assert(condition->type == AT_VALUE && "Condition resolved to not value!");
    if (is_truthy(&condition->value)) {
        interpret(ctx, _if.body);
    }

    return make_empty(ast->token);
}

Ast *interpret_unary(Context *ctx, Ast *ast)
{
    Unary *unary = &ast->unary;
    Ast *value = interpret(ctx, unary->expr);
    if (value->type == AT_VALUE)
    {
        Value val = value->value;
        if (value->type == T_NUMBER)
        {
            val.n = -1 * val.n;
            // we need to assign it back
            value->value = val;
        }
    }

    return value;
}

Ast *interpret_binop(Context *ctx, Ast *ast)
{
    BinOp *binop = &ast->op;
    Ast *left = interpret(ctx, binop->lvalue);
    Ast *right = interpret(ctx, binop->rvalue);

    if (left->type != AT_VALUE || right->type != AT_VALUE)
    {
        print_compiler_error(
            left->token,
            "You can't perform binary opeartions on not values that are not hadling it!"
        );
        exit(EXIT_FAILURE);
    }

    if (left->type != right->type)
    {
        print_compiler_error(left->token, "You are trying to perform operations on values that are not the same type!");
        exit(EXIT_FAILURE);
    }

    Value result;
    Value lval = left->value;
    Value rval = right->value;
    enum ValueType type = lval.type;
    result.type = type;
    if (binop->type == T_PLUS)
    {
        if (type == AV_STRING)
        {
            size_t concatenated_size = lval.str->size + rval.str->size;
            char *concatenated = malloc(concatenated_size + 1);
            memcpy(concatenated, lval.str->text, lval.str->size);
            memcpy(&concatenated[lval.str->size], rval.str->text, rval.str->size);

            concatenated[concatenated_size + 1] = 0;
            String *str = malloc(sizeof(String));
            str->text = concatenated;
            str->size = concatenated_size;
            result.str = str;
        }
        else
        {
            result.n = lval.n + rval.n;
        }
    }
    else if (binop->type == T_MINUS)
    {
        result.n = lval.n - rval.n;
    }
    else if (binop->type == T_STAR)
    {
        result.n = lval.n * rval.n;
    }
    else if (binop->type == T_SLASH)
    {
        if (rval.n == 0)
        {
            print_compiler_error(right->token, "You can't do division by 0!");
            exit(EXIT_FAILURE);
        }

        result.n = lval.n / rval.n;
    } else if (binop->type == T_PERCENT) {
        result.n = (int)lval.n % (int)rval.n;
    }

    return make_value(ast->token, &result);
}

Ast *interpret_assigment(Context *ctx, Ast *ast)
{
    Assigment *assigment = &ast->assigment;
    Variable *var = &assigment->var->var;
    ContextVariable *ctx_var = find_in_context(
        ctx,
        &var->name
    );
    if (ctx_var)
    {
        if (var->is_const)
        {
            print_compiler_error(assigment->var->token, "You can't reassign const value!");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        ctx_var = (ContextVariable *)malloc(sizeof(ContextVariable));
        ctx_var->var = assigment->var;
        push_to_context(ctx, ctx_var);
    }
    ctx_var->value = interpret(ctx, assigment->expr);
    return NULL;
}

Ast *interpret_variable(Context *ctx, Ast *expr)
{
    Variable *variable = &expr->var;
    ContextVariable *var = find_in_context(ctx, &variable->name);
    if (var)
    {
        return var->value;
    }

    char *error_message = malloc(1024);
    sprintf(
        error_message,
        "Undefined variable `%.*s`",
        (int)variable->name.size,
        variable->name.text);

    print_compiler_error(expr->token, error_message);
    free(error_message);
    exit(EXIT_FAILURE);
}

Ast *interpret_statement_list(Context *ctx, Ast *ast)
{
    StatementList *stmtList = &ast->stmt_list;
    Ast *stmt = stmtList->statements;
    while (stmt)
    {
        interpret(ctx, stmt);
        stmt = stmt->next;
    }

    return NULL;
}

Ast *user_defined_function_runner(Context *ctx, Ast *fn_call, UserDefinedFunction *user_fn)
{
    FunctionCall *fnCall = &fn_call->function_call;
    if (fnCall->argc != user_fn->argc)
    {
        print_compiler_error(fn_call->token, "Argument count do not match!");
        exit(EXIT_FAILURE);
    }

    Context stack;
    stack.vars = NULL;
    stack.parent = ctx;
    if (fnCall->args->type != AT_EMPTY) {
        size_t argc = fnCall->argc;
        Ast *param = user_fn->parameters->expr_list.exprs;
        Ast *arg = fnCall->args->expr_list.exprs;
        while(argc--)
        {
            ContextVariable *ctx_var = (ContextVariable *)malloc(sizeof(ContextVariable));
            ctx_var->var = param;
            ctx_var->value = interpret(ctx, arg);
            push_to_context(&stack, ctx_var);
            param = param->next;
            arg = arg->next;
        }
    }

    StatementList *stmt_list = &user_fn->statements->stmt_list;
    Ast *statement = stmt_list->statements;
    while(statement) {
        interpret(&stack, statement);
        statement = statement->next;
    }

    // Drop stack.
    if (stack.vars != NULL) {
        ContextVariable *var = stack.vars;
        while (var) {
            ContextVariable *tmp = var->next;            
            free(var);
            var = tmp;
        }
    }

    return make_empty(fn_call->token);
}

Ast *interpret_function_call(Context *ctx, Ast *expr)
{
    FunctionCall *fnCall = &expr->function_call;
    String *fnName = fnCall->identifier;
    Function *function = find_function(fnName);
    if (function == NULL)
    {
        char *buffer = malloc(1024);
        sprintf(
            buffer,
            "Function `%.*s` doesn't exist!",
            (int)fnName->size,
            fnName->text);
        print_compiler_error(expr->token, buffer);
        free(buffer);
        exit(EXIT_FAILURE);
    }

    Context fn_ctx;
    fn_ctx.vars = NULL;
    fn_ctx.parent = ctx;

    if (function->type == F_BUILTIN)
    {
        return function->bfunc.fp(&fn_ctx, fnCall->args);
    }

    if (function->type == F_USER_DEFINED)
    {
        return user_defined_function_runner(&fn_ctx, expr, &function->ufunc);
    }

    return NULL;
}

Ast *interpret_function_definition(Context *ctx, Ast *ast)
{
    (void)ctx;
    Function *fun = malloc(sizeof(Function));
    FunctionDefinition *fn_def = &ast->function_definition;
    fun->type = F_USER_DEFINED;
    fun->name = *fn_def->name;
    fun->ufunc = (UserDefinedFunction){
        .parameters = fn_def->parameters,
        .statements = fn_def->statementList,
        .argc = fn_def->argc
    };

    add_function(fun);
    return NULL;
}

Ast *interpret_logical(Context *ctx, Ast *ast)
{
    Value value;
    value.type = AV_BOOL;
    value.b = false;
    Logical *logical = &ast->logical;
    Ast *left = interpret(ctx, logical->left);
    Ast *right = interpret(ctx, logical->right);
    Value *lval = &left->value;
    Value *rval = &right->value;
    if (lval->type != rval->type) {
        return make_value(ast->token, &value);
    }

    if (logical->type == T_CMP) {
        if (is_equal(lval, rval)) {
            value.b = true;    
        }
    }
    if (logical->type == T_BANG_EQUAL) {
        if (!is_equal(lval, rval)) {
            value.b = true;    
        }
    }
    if (lval->type != AV_NUMBER) {
        return make_value(ast->token, &value);
    }
    if (logical->type == T_GREATER) {
        if (lval->n > rval->n) {
            value.b = true;    
        }
    }
    
    if (logical->type == T_GREATER_EQUAL) {
        if (lval->n >= rval->n) {
            value.b = true;    
        }
    }

    if (logical->type == T_LESS) {
        if (lval->n < rval->n) {
            value.b = true;    
        }
    }

    if (logical->type == T_LESS_EQUAL) {
        if (lval->n <= rval->n) {
            value.b = true;    
        }
    }

    return make_value(ast->token, &value);
}

Ast *interpret(Context *ctx, Ast *ast)
{
    if (ast->type == AT_IF) return interpret_if(ctx, ast);
    if (ast->type == AT_EMPTY)
        return ast;
    if (ast->type == AT_VALUE)
        return ast;
    if (ast->type == AT_ASSIGMENT)
        return interpret_assigment(ctx, ast);
    if (ast->type == AT_VARIABLE)
        return interpret_variable(ctx, ast);
    if (ast->type == AT_BINOP)
        return interpret_binop(ctx, ast);
    if (ast->type == AT_UNARY)
        return interpret_unary(ctx, ast);
    if (ast->type == AT_STATEMENT_LIST)
        return interpret_statement_list(ctx, ast);
    if (ast->type == AT_FN_CALL)
        return interpret_function_call(ctx, ast);
    if (ast->type == AT_FN_DEFINITION)
        return interpret_function_definition(ctx, ast);
    if (ast->type == AT_LOGICAL)
        return interpret_logical(ctx, ast);
    if (ast->type == AT_EXPR_LIST)
        return ast;

    print_compiler_error(ast->token, "Unknown type to interpret!");

    exit(EXIT_FAILURE);
}
