#ifndef __TOKEN_STREAM
#define __TOKEN_STREAM

struct VM;

enum TokenType {
      // Single-character tokens.
  LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
  COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

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
    int line_start_position;
    int stream_start_position;
    int stream_end_position;
} Token_Metadata;

typedef struct Token {
    enum TokenType type;
    char * value;
    Token_Metadata meta_data;

    struct Token *next;
} Token;

#endif
