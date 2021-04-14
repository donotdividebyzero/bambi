#ifndef __PARSER_H
#define __PARSER_H

#include "token.h"
#include "ast.h"

#define EAT(token) \
Tokenizer_Error is_eaten = eat_token(token); \
    if (is_eaten.error != NULL) {\
        return make_error(is_eaten.error);\
    }

// NUMERIC | LEFT_PAREN expr RIGHT_PAREN
Ast *factor();

// factor ((MUL | DIV) factor)*
Ast *term();

// term ((PLUS | MINUS) term)*
Ast *expr();

#endif