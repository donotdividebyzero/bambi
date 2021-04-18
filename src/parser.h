#ifndef __PARSER_H
#define __PARSER_H

#include "token.h"
#include "ast.h"

#define EAT(token) {\
Tokenizer_Error is_eaten = eat_token(token); \
    if (is_eaten.error != NULL) {\
        return make_error(is_eaten.error);\
    }\
}\

Ast *factor();
Ast *term();
Ast *expr();
Ast *variable();
Ast *statement();
Ast *statement_list();
Ast *compound_statement();
Ast *assigment_statement();
Ast *empty();
Ast *program();

#endif