#include "ast.h"
#include <stdio.h>

void free_ast_value(Ast *node)
{
    if (node->value->type == AV_STRING)
    {
        free(node->value->str->value);
        free(node->value->str);
    }

    if (node->value->type == AV_NUMERIC)
    {
        free(node->value->numeric);
    }

    free(node);
}

void free_ast_error(Ast *node)
{
    if (node->error != NULL)
    {
        free(node->error);
        free(node);
    }
}

void free_ast(Ast *node) 
{
    if (node->error != NULL)
    {
        free_ast_error(node);
    }
    if (node->type == AT_VALUE) free_ast_value(node);
}

char *ast_type_to_str(enum Ast_Type type)
{
    if (type == AT_VALUE) return "AT_VALUE";
    if (type == AT_BINOP) return "AT_BINOP";
    if (type == AT_UNARY) return "AT_UNARY";
    if (type == AT_ASSIGMENT) return "AT_ASSIGMENT";
    if (type == AT_VARIABLE) return "AT_VARIABLE";
    if (type == AT_EMPTY) return "AT_EMPTY";
    if (type == AT_STATEMENT_LIST) return "AT_STATEMENT_LIST";
    return NULL;
}

Ast *make_unary(enum Ast_Unary_Type type, Ast *expr)
{
    Ast *node = malloc(sizeof(Ast));
    node->type = AT_UNARY;
    Ast_Unary *unary = malloc(sizeof(Ast_Unary));
    node->unary = unary;
    node->unary->type = type;
    node->unary->expr = expr;
    return node;
}

Ast *make_error(char *err)
{
    Ast *node = malloc(sizeof(Ast));
    size_t err_length = strlen(err);
    node->error = malloc(sizeof(err_length));
    strncpy(node->error, err, err_length);

    return node;
}

Ast *make_numeric(Ast_Numeric *number)
{
    Ast_Value *value = malloc(sizeof(Ast_Value));
    value->type = AV_NUMERIC;
    value->numeric = number; 

    Ast *node = malloc(sizeof(Ast));
    node->type = AT_VALUE;
    node->value = value;

    return node;
}

Ast *make_integer(char *val)
{
    Ast_Numeric *integer = malloc(sizeof(Ast_Numeric));
    integer->type = AN_INTEGER;
    integer->l = strtol(val, NULL, 10);
    return make_numeric(integer);
}

Ast *make_float(char *val)
{
    Ast_Numeric *floaty = malloc(sizeof(Ast_Numeric));
    floaty->type = AN_FLOAT;
    floaty->f = strtof(val, NULL);
    return make_numeric(floaty);
}

Ast *make_string(char *val)
{
    Ast_Value *value = malloc(sizeof(Ast_Value));
    value->type = AV_STRING;

    Ast_String *string = malloc(sizeof(Ast_String));
    size_t val_length = strlen(val);
    string->value = malloc(sizeof(val_length));
    string->length = val_length;

    strncpy(string->value, val, val_length);

    value->str = string;
    
    Ast *node = malloc(sizeof(Ast));
    node->type = AT_VALUE;
    node->value = value;
    return node;
}

Ast *make_binop(enum Ast_BinOp_Enum type, Ast *lval, Ast *rval) 
{
    Ast *node = malloc(sizeof(Ast));
    node->type = AT_BINOP;

    Ast_BinOp *binop = malloc(sizeof(Ast_BinOp));
    binop->type = type;
    binop->lvalue = lval;
    binop->rvalue = rval;

    node->op = binop;
    return node;
}

Ast *make_variable(char *name, int is_const)
{
    Ast *node = malloc(sizeof(Ast));
    node->type = AT_VARIABLE;

    Ast_Variable *var = malloc(sizeof(Ast_Variable));
    var->name = malloc(strlen(name));
    strncpy(var->name, name, strlen(name));
    var->is_const = is_const;
    node->var = var;

    return node;
}

Ast *make_empty()
{
    Ast *node = malloc(sizeof(Ast));
    node->type = AT_EMPTY;
    return node;
}

Ast *make_assigment(Ast* var, Ast *expr)
{
    Ast *node = malloc(sizeof(Ast));
    node->type = AT_ASSIGMENT;
    Ast_Assigment *assigment = malloc(sizeof(Ast_Assigment));
    assigment->expr = expr;
    assigment->var = var;

    node->assigment = assigment;
    return node;
}

Ast *make_statement_list(Ast *root, struct Ast *stmt)
{
    if (stmt == NULL) {
        return make_error("Statment can't be NULL it can be an empty statement.");
    }

    if (root == NULL) {
        root = malloc(sizeof(Ast));
        root->type = AT_STATEMENT_LIST;
        root->stmt_list = malloc(sizeof(Ast_Statement_List));
        root->stmt_list->statements = malloc(sizeof(Ast));
        root->stmt_list->size = 1;
        root->stmt_list->statements[root->stmt_list->size-1] = stmt;
        return root;
    }

    root->stmt_list->size = root->stmt_list->size + 1;
    root->stmt_list->statements = realloc(root->stmt_list->statements, (root->stmt_list->size * sizeof(Ast)));
    root->stmt_list->statements[root->stmt_list->size-1] = stmt;
    
    return root;
}