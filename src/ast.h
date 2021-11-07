#ifndef __BAMBI_AST
#define __BAMBI_AST
#include "types.h"

Ast *make_function_definition(Token *, Ast *, Ast *);
Ast *make_function_call(Token *, Ast*);
Ast *make_statement_list(Token *, Ast *, Ast *);
Ast *make_expr_list(Token *, Ast *);
Ast *make_assigment(Token *, Ast *, Ast *);
Ast *make_variable(Token *, bool);
Ast *make_value(Token *, Value*);
Ast *make_logical(Token *, Ast *, Ast *);
Ast *make_if(Token *, Ast *, Ast *);
Ast *make_binop(Token *, Ast *, Ast *);
Ast *make_unary(Token *, Ast *);
Ast *make_empty(Token *);

const char *ast_type_to_str(enum AstType);
#endif
