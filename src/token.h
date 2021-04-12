#ifndef __TOKEN_H
#define __TOKEN_H

#include <stdio.h>

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
  IDENTIFIER, STRING, NUMBER,

  // Keywords.
  IF, ELSE,
  TRUE, FALSE, NIL,
  FN, RETURN,
  FOR, WHILE, 
  LET, CONST
};

typedef struct Token_Metadata {
    int line;
    int line_position;
} Token_Metadata;

typedef struct Token {
    enum TokenType type;
    char *value;
    Token_Metadata *meta_data;

    struct Token *next;
} Token;

Token *token_stream_tokenize(FILE *);

#endif
