#ifndef __AST_H
#define __AST_H
#include <stdlib.h>
#include <string.h>

struct Ast;

#if 1
    #define DEBUG(text, ...) fprintf(stdout, text, __VA_ARGS__);
#else
    #define DEBUG(text, ...)
#endif

#define ASSERT_ERROR(node) if(node->error != NULL) return node;

enum Ast_Numeric_Type {
    AN_FLOAT,
    AN_INTEGER
};
typedef struct Ast_Numeric {
    enum Ast_Numeric_Type type;
    union {
        long l;
        float f;
    };
} Ast_Numeric;

typedef struct Ast_String {
    char *value;
    size_t length;
} Ast_String;

enum Ast_Value_Enum {
    AV_NUMERIC,
    AV_STRING,
    AV_NIL,
    AV_BOOL
};

typedef struct Ast_Value {
    enum Ast_Value_Enum type;
    union {
        int bool;
        Ast_String *str;
        Ast_Numeric *numeric;
    };
} Ast_Value;

enum Ast_BinOp_Enum {
    BINOP_ADD,
    BINOP_SUB,
    BINOP_MUL,
    BINOP_DIV,
    BINOP_MOD
};

typedef struct Ast_BinOp
{
    enum Ast_BinOp_Enum type;
    struct Ast *lvalue;
    struct Ast *rvalue;
} Ast_BinOp;

enum Ast_Type {
    AT_VALUE,
    AT_BINOP
};

typedef struct Ast
{
    char *error;
    enum Ast_Type type;
    union {
        Ast_Value *value;
        Ast_BinOp *op;
    };
} Ast;

Ast *make_error(char *);
Ast *make_integer(char *);
Ast *make_float(char *);
Ast *make_string(char *);
Ast *make_binop(enum Ast_BinOp_Enum, Ast *, Ast *);
Ast *make_numeric(Ast_Numeric *);

char *ast_type_to_str(enum Ast_Type);

#endif