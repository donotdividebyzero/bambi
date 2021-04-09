#ifndef __TOKEN_STREAM
#define __TOKEN_STREAM
#include <stdlib.h>

enum TokenType {
	INTEGER,
	PLUS
};

struct TokenStream {
	enum TokenType type;
	char *c;
	struct TokenStream *next;
};

void token_stream_push(struct TokenStream *, char *);

void token_stream_print(struct TokenStream *);

char *token_stream_peek(struct TokenStream *);

char *token_stream_eat(struct TokenStream *, enum TokenType);

void token_stream_clean(struct TokenStream *);

#endif
