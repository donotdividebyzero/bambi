#ifndef __BAMBI_TYPES
#define __BAMBI_TYPES
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct String {
    size_t size;
    const char* text;
};

typedef struct String String;


enum TokenType {
  T_EOF,
  // Single-character tokens.
  T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE,
  T_COMMA, T_DOT, T_MINUS, T_PLUS, T_SEMICOLON, T_COLON, T_SLASH, T_STAR, T_PERCENT, T_AT,

  // One or two character tokens.
  T_ASSIGN,
  T_BANG, T_BANG_EQUAL,
  T_EQUAL, T_CMP, 
  T_GREATER, T_GREATER_EQUAL,
  T_LESS, T_LESS_EQUAL,
  T_AND, T_OR,

  // Bitwise
  T_BITWISE_AND, T_BITWISE_OR,

  // Literals.
  T_IDENTIFIER, T_STRING,
  T_NUMBER,
  T_TRUE, T_FALSE, T_NIL,

  // Keywords 
  T_IF, T_ELSE,
  T_FOR, T_WHILE, T_BREAK, T_CONTINUE,
  T_CONST,
  T_FN, T_STRUCT,
  T_RETURN
};

struct TokenKeyword {
    enum TokenType token_type;
    String value;
};
typedef struct TokenKeyword TokenKeyword;

struct FileLocation {
    const char *file_name;
    int line;
    int column;
};
typedef struct FileLocation FileLocation;

struct Lexer;

struct Token {
    enum TokenType type;
    struct Lexer *lexer;
    String value;
    FileLocation location;    
    struct Token *next;
};

typedef struct Token Token;

struct SourceCodeFile {
    const char *file_name;
    const char *content;
    const char **lines;
    int line;
    int column;
    size_t size;
};
typedef struct SourceCodeFile SourceCodeFile;

struct Lexer {
    SourceCodeFile *file;
    int current;
    Token *tokens;
    Token *current_token;
};
typedef struct Lexer Lexer;

struct Ast;

typedef struct Unary {
    struct Ast *expr;
} Unary;

enum ValueType {
    AV_NUMBER,
    AV_STRING,
    AV_BOOL,
    AV_NIL
};

struct Value {
    enum ValueType type;
    union {
        bool b;
        String *str;
        float n;
    };
};
typedef struct Value Value;

typedef struct BinOp {
    enum TokenType type;
    struct Ast *lvalue;
    struct Ast *rvalue;
} BinOp;

typedef struct Variable {
    bool is_const;
    String name;
} Variable;

typedef struct Assigment {
    struct Ast *var;
    struct Ast *expr;
} Assigment;

struct If {
   struct Ast *body;
   struct Ast *condition;
};
typedef struct If If;

struct Return 
{
    struct Ast *expr;
};
typedef struct Return Return;

struct Argument {
    struct Ast *name;
    enum ValueType type;
    String *typeIdentifier;
};

typedef struct Argument Argument;

struct For {
   struct Ast *body;
   struct Ast *condition;
};
typedef struct For For;

struct While {
   struct Ast *body;
   struct Ast *condition;
};
typedef struct While While;

typedef struct StatementList {
    struct Ast *statements;
} StatementList;

struct FunctionCall {
    String *identifier;
    size_t argc;
    struct Ast *args;     
};

typedef struct FunctionCall FunctionCall;

struct FunctionDefinition {
    String *name;
    size_t argc;
    struct Ast *parameters;
    struct Ast *statementList;
};

typedef struct FunctionDefinition FunctionDefinition;

struct Logical {
    enum TokenType type;
    struct Ast *left;
    struct Ast *right;
};
typedef struct Logical Logical;

struct ExprList
{
    struct Ast *exprs;
};

typedef struct ExprList ExprList;

enum AstType {
    AT_BREAK,
    AT_CONTINUE,
    AT_RETURN,
    AT_WHILE, 
    AT_FOR,
    AT_IF,
    AT_EXPR_LIST,
    AT_VALUE,
    AT_ASSIGMENT,
    AT_VARIABLE,
    AT_BINOP,
    AT_LOGICAL,
    AT_UNARY,
    AT_EMPTY,
    AT_STATEMENT_LIST,
    AT_FN_CALL,
    AT_FN_DEFINITION
};

typedef struct Ast
{
    Token *token;
    enum AstType type;
    union {
        Return _return;
        While _while;
        For _for;
        If _if;
        ExprList expr_list;
        Unary unary;
        Value value;
        BinOp op;
        Logical logical;
        Variable var;
        Assigment assigment;
        StatementList stmt_list;
        FunctionCall function_call;
        FunctionDefinition function_definition;
    };

    struct Ast *next; // this parameter is only here if we want to have a list
} Ast;

struct ContextVariable {
    Ast *var;
    Ast *value;
    struct ContextVariable *next;
};
typedef struct ContextVariable ContextVariable;

struct Context {
    struct Context *parent;
    ContextVariable *vars;
};
typedef struct Context Context;

struct BuiltInFunction {
    size_t argc;
    Ast *(*fp)(Context *, Ast *);
};

typedef struct BuiltInFunction BuiltInFunction;

struct UserDefinedFunction {
    Token *token;
    size_t argc;
    Ast *parameters;
    Ast *statements;
};

typedef struct UserDefinedFunction UserDefinedFunction;

enum FunctionType {
    F_BUILTIN,
    F_USER_DEFINED
};

struct Function {
    enum FunctionType type;
    String name;
    union {
        UserDefinedFunction ufunc;
        BuiltInFunction bfunc;
    };

    struct Function *next;
};

typedef struct Function Function;
#endif
