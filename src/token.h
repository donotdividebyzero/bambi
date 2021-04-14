#ifndef __TOKEN_H
#define __TOKEN_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

enum TokenType {
      // Single-character tokens.
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, COLON, SLASH, STAR,

  // One or two character tokens.
  ASSIGN,
  BANG, BANG_EQUAL,
  EQUAL, EQUAL_EQUAL,
  GREATER, GREATER_EQUAL,
  LESS, LESS_EQUAL,
  AND, OR,

  // Literals.
  IDENTIFIER, STRING, INTEGER, FLOAT, NUMBER,

  // Keywords.
  IF, ELSE,
  TRUE, FALSE, NIL,
  FN, RETURN,
  FOR, WHILE, 
  LET, CONST
};

typedef struct Token_Metadata {
    size_t line;
    size_t line_position;
    size_t length;
} Token_Metadata;

typedef struct Token {
    enum TokenType type;
    char *value;
    int allocated;
    Token_Metadata *meta_data;

    struct Token *next;
} Token;

typedef struct Tokenizer_Error {
    char *error;
} Tokenizer_Error;

Tokenizer_Error tokenize(FILE *);

Token *next_token();

Token *current_token();

Tokenizer_Error eat_token(enum TokenType);

Tokenizer_Error token_stream_shutdown();

char *token_type_to_str(enum TokenType);

#endif
