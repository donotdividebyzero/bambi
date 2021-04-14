#include "ast.h"


char *ast_type_to_str(enum Ast_Type type)
{
    if (type == AT_VALUE) return "AT_VALUE";
    if (type == AT_BINOP) return "AT_BINOP";
    return NULL;
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