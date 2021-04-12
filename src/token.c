#include "token.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Token *token_head;
Token *current;

void push_token(Token_Metadata *meta_data, enum TokenType type, char* value)
{
    Token *t = malloc(sizeof(Token));
    t->meta_data = meta_data;
    t->type = type;
    char * v = malloc(strlen(value));
    strncpy(v, value, strlen(value));
    t->value = v;

    if (token_head == NULL) {
        token_head = t;
        current = token_head;
    } else {
        current->next = t;
        current = current->next;
    }
}

Token *token_stream_tokenize(FILE *input)
{
    int line = 1;
    int line_position = 1;
    char c;
    while((c = fgetc(input)) != EOF) {
        Token_Metadata *meta = malloc(sizeof(Token_Metadata));
        meta->line = line;
        meta->line_position = line_position;
        switch (c) {
            case '{': push_token(meta, LEFT_BRACE, &c); break;
            case '}': push_token(meta, RIGHT_BRACE, &c); break;
            case '(': push_token(meta, LEFT_PAREN, &c); break;
            case ')': push_token(meta, RIGHT_PAREN, &c); break;
            case '+': push_token(meta, PLUS, &c); break;
            case '-': push_token(meta, MINUS, &c); break;
            case '*': push_token(meta, STAR, &c); break;
            case '/': push_token(meta, SLASH, &c); break;
            case '.': push_token(meta, DOT, &c); break;
            case ',': push_token(meta, COMMA, &c); break;
            case ';': push_token(meta, SEMICOLON, &c); break;
            case ':': push_token(meta, COLON, &c); break;
            case '!': push_token(meta, BANG, &c); break;
            case '=': push_token(meta, EQUAL, &c); break;
            case '>': push_token(meta, GREATER, &c); break;
            case '<': push_token(meta, LESS, &c); break;
            case '\n': {
                ++line;
                line_position = 1;
            } break;
            default: break;
        }
        ++line_position;
    }

    return token_head;
}

void token_stream_shutdown()
{  
    Token *tmp;
    while (token_head != NULL) {
        tmp = token_head->next;
        free(token_head->value);
        token_head = tmp;
    }
}
