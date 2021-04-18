#ifndef __AST_H
#define __AST_H
#include <stdlib.h>
#include <string.h>

struct Ast;

#define ASSERT_ERROR(node) if(node->error != NULL) return node;

enum Ast_Unary_Type {
    AU_PLUS,
    AU_MINUS
};

typedef struct Ast_Unary {
    enum Ast_Unary_Type type;
    struct Ast *expr;
} Ast_Unary;

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

typedef struct Ast_Variable {
    int is_const;
    char *name;
} Ast_Variable;

typedef struct Ast_Assigment
{
    struct Ast *var;
    struct Ast *expr;
} Ast_Assigment;

typedef struct Ast_Statement_List 
{
    size_t size;
    struct Ast **statements;
} Ast_Statement_List;

enum Ast_Type {
    AT_VALUE,
    AT_ASSIGMENT,
    AT_VARIABLE,
    AT_BINOP,
    AT_UNARY,
    AT_EMPTY,
    AT_STATEMENT_LIST
};

typedef struct Ast
{
    char *error;
    enum Ast_Type type;
    union {
        Ast_Unary *unary;
        Ast_Value *value;
        Ast_BinOp *op;
        Ast_Variable *var;
        Ast_Assigment *assigment;
        Ast_Statement_List *stmt_list;
    };
} Ast;

Ast *make_statement_list(Ast *, Ast *);
Ast *make_assigment(Ast *, Ast *);
Ast *make_variable(char *, int);
Ast *make_error(char *);
Ast *make_integer(char *);
Ast *make_float(char *);
Ast *make_string(char *);
Ast *make_binop(enum Ast_BinOp_Enum, Ast *, Ast *);
Ast *make_numeric(Ast_Numeric *);
Ast *make_unary(enum Ast_Unary_Type, Ast *);
Ast *make_empty();

char *ast_type_to_str(enum Ast_Type);

void free_ast(Ast *);

#endif