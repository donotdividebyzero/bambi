#ifndef __BAMBI_TOKENIZER
#define __BAMBI_TOKENIZER
#include "vm.h"

void tokenize(Lexer *);
void print_token(Token *);
void debug_tokenize(Lexer *);
const char *token_type_to_str(enum TokenType);

#endif
