#ifndef __TOKEN_H
#define __TOKEN_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "string.h"

enum TokenType {
  T_EOF,
      // Single-character tokens.
  T_LPAREN, T_RPAREN, T_LBRACE, T_RBRACE,
  T_COMMA, T_DOT, T_MINUS, T_PLUS, T_SEMICOLON, T_COLON, T_SLASH, T_STAR,

  // One or two character tokens.
  T_ASSIGN,
  T_BRANG, T_BANG_EQUAL,
  T_EQUAL,
  T_GREATER, T_GREATER_EQUAL,
  T_LESS, T_LESS_EQUAL,
  T_AND, T_OR,

  // Literals.
  T_IDENTIFIER, T_STRING,
  T_INTEGER, T_FLOAT,

  // Keywords 
  T_IF, T_ELSE,
  T_TRUE, T_FALSE, T_NIL,
  T_FOR, T_WHILE,
  T_CONST,
  T_FN, T_STRUCT
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
