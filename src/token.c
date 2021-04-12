#include "token.h"
#include <stdlib.h>
#include <stdio.h>

Token *token_head;
Token *current;

void push_token(Token_Metadata *meta_data, enum TokenType type, char* value)
{
    Token *t = malloc(sizeof(Token));
    t->meta_data = meta_data;
    t->type = type;
    t->value = value;

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
            case '{': push_token(meta, LEFT_BRACE, NULL); break;
            case '}': push_token(meta, RIGHT_BRACE, NULL); break;
            case '(': push_token(meta, LEFT_PAREN, NULL); break;
            case ')': push_token(meta, RIGHT_PAREN, NULL); break;
            case '+': push_token(meta, PLUS, NULL); break;
            case '-': push_token(meta, MINUS, NULL); break;
            case '*': push_token(meta, STAR, NULL); break;
            case '/': push_token(meta, SLASH, NULL); break;
            case '.': push_token(meta, DOT, NULL); break;
            case ',': push_token(meta, COMMA, NULL); break;
            case ';': push_token(meta, SEMICOLON, NULL); break;
            case ':': push_token(meta, COLON, NULL); break;
            case '!': push_token(meta, BANG, NULL); break;
            case '=': push_token(meta, EQUAL, NULL); break;
            case '>': push_token(meta, GREATER, NULL); break;
            case '<': push_token(meta, LESS, NULL); break;
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
