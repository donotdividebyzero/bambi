#ifndef __BAMBI_PARSER
#define __BAMBI_PARSER
#include "vm.h"

Ast *factor(Lexer *);
Ast *term(Lexer *);
Ast *expr(Lexer *);
Ast *expr_list(Lexer *);
Ast *or(Lexer *);
Ast *and(Lexer *);
Ast *_if(Lexer *);
Ast *equal(Lexer *);
Ast *variable(Lexer *);
Ast *statement(Lexer *);
Ast *statement_list(Lexer *);
Ast *function_call_expr(Lexer *);
Ast *compound_statement(Lexer *);
Ast *assigment(Lexer *);
Ast *empty(Lexer *);
Ast *program(Lexer *);

#endif
