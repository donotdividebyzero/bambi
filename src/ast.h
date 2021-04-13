#ifndef __AST_H
#define __AST_H

enum Ast_Number_Enum {
    A_FLOAT,
    A_INTEGER
};

typedef struct Ast_Number {
    enum Ast_Number_Enum type;
    union {
        long l;
        float f;
    };
} Ast_Number;

enum Ast_Operation_Enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD
};

typedef struct Ast_Operation
{
   enum  Ast_Operation_Enum type;
} Ast_Operation;

typedef struct Ast
{
    char *error;
    union {
        Ast_Number number;
        Ast_Operation op;
    };
} Ast;


#endif