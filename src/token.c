#include "token.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

Token *token_head;
Token *current;

int isalphanum(char c)
{
    return isdigit(c) || isalpha(c);
}

char ts_peek(FILE *stream)
{
    int c = fgetc(stream);
    ungetc(c, stream);
    return (char)c;
}

void ts_advance(FILE *stream)
{
    fgetc(stream);
}

void push_token(Token_Metadata *meta_data, enum TokenType type, char* value)
{
    Token *t = malloc(sizeof(Token));
    t->meta_data = meta_data;
    t->type = type;
    t->allocated = 0;

    if (value != NULL) {
        t->value = value;
        t->allocated = 1;
    }
    
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
        meta->length = 1;
        meta->line_position = line_position;
        switch (c) {
            case '{': {push_token(meta, LEFT_BRACE, NULL);} break;
            case '}': {push_token(meta, RIGHT_BRACE, NULL);} break;
            case '(': {push_token(meta, LEFT_PAREN, NULL);} break;
            case ')': {push_token(meta, RIGHT_PAREN, NULL);} break;
            case '+': {push_token(meta, PLUS, NULL);} break;
            case '-': {push_token(meta, MINUS, NULL);} break;
            case '*': {push_token(meta, STAR, NULL);} break;
            case '.': {push_token(meta, DOT, NULL);} break;
            case ',': {push_token(meta, COMMA, NULL);} break;
            case ';': {push_token(meta, SEMICOLON, NULL);} break;
            case '/': {
                char tmp = ts_peek(input);
                if (tmp == '/') {
                    ts_advance(input); // this case of single line comment, just remove every thing up to new line. Just like this one :D
                    for(;;) {
                        tmp = fgetc(input);
                        if (tmp == '\n') break;
                    }
                }
            } break;
            case ':': {
                char tmp = ts_peek(input);
                if (tmp == '=') { meta->length = 2; push_token(meta, ASSIGN, NULL); ts_advance(input); }
                else { push_token(meta, COLON, NULL); }
            } break;
            case '!': {
                char tmp = ts_peek(input);
                if (tmp == '='){ meta->length = 2; push_token(meta, BANG_EQUAL, NULL); ts_advance(input); }
                else { push_token(meta, BANG, NULL); }
            } break;
            case '=': push_token(meta, EQUAL, NULL); break;
            case '>': {
                char tmp = ts_peek(input);
                if (tmp == '=') { meta->length = 2; push_token(meta, GREATER_EQUAL, NULL); ts_advance(input); }
                else { push_token(meta, GREATER, NULL); }
            } break;
            case '<': {
                char tmp = ts_peek(input);
                if (tmp == '=') { meta->length = 2; push_token(meta, LESS_EQUAL, NULL); ts_advance(input); }
                else { push_token(meta, LESS, NULL); }
            } break;
            case '"': {
                char tmp;
                size_t buffer_size = 1024;
                char *buffer = malloc(buffer_size);
                size_t count = 0;
                for(;;) {
                    tmp = fgetc(input);
                    if (isalphanum(tmp) || isspace(tmp)) {
                        if (count >= buffer_size) {
                            buffer_size = buffer_size + 1024;
                            buffer = realloc(buffer, buffer_size);
                        }
                        buffer[count++] = tmp;
                    }
                    if (tmp == '\n') {
                        ++line;
                        line_position = 1;
                        continue;
                    }
                    if (tmp == '"') {
                        ts_advance(input);
                        buffer[count] = '\n';
                        break;
                    }
                    ++line_position;
                }
                meta->length = count;
                push_token(meta, STRING, buffer);
            } break;
            case '\n': {
                ++line;
                line_position = 1;
            } break;
            default: {} break;
        }

        if (isspace(c)) continue;

        //read identifier
        if (isalpha(c)) {
            size_t buffer_size = 1024;
            char *buffer = malloc(buffer_size);
            size_t count = 0;
            buffer[count++] = c;
            for(;;) {
                char tmp = fgetc(input);
                if (isalpha(tmp)) {
                    if (count >= buffer_size) {
                        buffer_size = buffer_size + 1024;
                        buffer = realloc(buffer, buffer_size);
                    }
                    buffer[count++] = tmp;
                }
                
                if (isspace(tmp)) {
                    buffer[count] = '\n';
                    break;
                }
            }
            meta->length = count;
            push_token(meta, IDENTIFIER, buffer);
        }

        // read number
        if (isdigit(c))
        {
            char tmp;
            size_t count = 0;
            size_t buffer_size = 1024;
            char *buffer = malloc(buffer_size);
            buffer[count++] = c;
            for(;;) {
                tmp = fgetc(input);
                if (isdigit(tmp) || tmp == '.') {
                    buffer[count++] = tmp;
                    ++line_position;
                } else {
                    break;
                }
            }
            buffer[count] = '\n';
            meta->length = count;
            meta->line_position = line_position;
            push_token(meta, NUMBER, buffer);
            continue;
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
        if (token_head->allocated == 1) { free(token_head->value); }
        free(token_head->meta_data);
        token_head = tmp;
    }
}
