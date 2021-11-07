#include "ast.h"
#include <string.h>

Ast *make_if(Token *token, Ast *condition, Ast *body)
{
    Ast *node = malloc(sizeof(Ast));
    node->token = token;
    node->type = AT_IF;
    node->_if = (If){body, condition};

    return node;
}

Ast *make_expr_list(Token *token, Ast *begin)
{
    Ast *node = malloc(sizeof(Ast));
    node->next = NULL;
    node->type = AT_EXPR_LIST;
    node->token = token;
    node->expr_list = (ExprList) {
        .exprs = begin
    };

    return node;
}

Ast *make_unary(Token *token, Ast *expr)
{
    Ast *node = malloc(sizeof(Ast));
    node->type = AT_UNARY;
    node->token = token;
    node->unary = (Unary) {
        .expr = expr
    };
    return node;
}

Ast *make_value(Token *token, Value *val) 
{
    Ast *node = malloc(sizeof(Ast));
    node->type = AT_VALUE;
    node->token = token;
    if (val) {
        node->value = *val;
    }
    else {
        node->value = (Value) {
            .type = AV_NIL
        };
    }

    return node;
}

Ast *make_binop(Token *token, Ast *lval, Ast *rval) 
{
    Ast *node = malloc(sizeof(Ast));
    node->type = AT_BINOP;
    node->token = token;

    node->op = (BinOp) {
        .type = token->type,
        .lvalue = lval,
        .rvalue = rval
    };

    return node;
}

Ast *make_variable(Token *token, bool is_const)
{
    Ast *node = malloc(sizeof(Ast));
    node->type = AT_VARIABLE;
    node->token = token;

    node->var = (Variable) {
        .name = token->value,
        .is_const = is_const
    };

    return node;
}

Ast *make_empty(Token *token)
{
    Ast *node = malloc(sizeof(Ast));
    node->type = AT_EMPTY;
    node->token = token;
    return node;
}

Ast *make_assigment(Token *token, Ast* var, Ast *expr)
{
    Ast *node = malloc(sizeof(Ast));
    node->type = AT_ASSIGMENT;
    node->token = token;

    node->assigment = (Assigment) {
        .expr = expr,
        .var = var
    };

    return node;
}

Ast *make_statement_list(Token *token, Ast *root, Ast *stmt)
{
    stmt->next = NULL;
    stmt->token = token;

    if (root == NULL) {
        root = malloc(sizeof(Ast));
        root->type = AT_STATEMENT_LIST;
        root->stmt_list.statements = stmt;
        return root;
    }
    
    Ast *stmt_loc = root->stmt_list.statements;
    while(stmt_loc->next) stmt_loc = stmt_loc->next;
    stmt_loc->next = stmt;
    
    return root;
}

Ast *make_function_call(Token *token, Ast *args)
{
   String *fnName = &token->value;
   Ast *node = (Ast *)malloc(sizeof(Ast)); 
   node->type = AT_FN_CALL;
   node->token = token;
   size_t argc = 0;
   if (args->type != AT_EMPTY) {
       Ast *arg = args->expr_list.exprs;
       while(arg) {
           argc++;
           arg = arg->next;
       }
   }
   
   node->function_call = (FunctionCall) {
        .identifier = fnName,
        .args = args,
        .argc = argc
   };

   return node;
}

Ast *make_function_definition(Token *definition_token, Ast *args, Ast *statements)
{
    Ast *node = malloc(sizeof(Ast));
    node->type = AT_FN_DEFINITION;
    size_t argc = 0;
    if (args->type != AT_EMPTY) {
        Ast *arg = args->expr_list.exprs;
        while(arg) {
            argc++;
            arg = arg->next;
        }
    }

    node->function_definition = (FunctionDefinition) {
        .name = &definition_token->value,
        .parameters = args,
        .argc = argc,
        .statementList = statements
    };

    return node;
}

Ast *make_logical(Token *token, Ast *l, Ast *r)
{
    Ast *node = malloc(sizeof(Ast));
    node->type = AT_LOGICAL;
    node->token = token;
    node->logical = (Logical) {
        .type = token->type,
        .left = l,
        .right = r
    };

    return node;
}
