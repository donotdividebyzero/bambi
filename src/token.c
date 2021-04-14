#include "token.h"

Token *token_head;
Token *current;

char *token_type_to_str(enum TokenType type) {
    if (type == PLUS) return "T_PLUS";
    if (type == MINUS) return "T_MINUS";
    if (type == LEFT_PAREN) return "T_LEFT_PAREN";
    if (type == RIGHT_PAREN) return "T_RIGHT_PAREN";
    if (type == LEFT_BRACE) return "T_LEFT_BRACE";
    if (type == RIGHT_BRACE) return "T_RIGHT_BRACE";
    if (type == COMMA) return "T_COMMA";
    if (type == DOT) return "T_DOT";
    if (type == SEMICOLON) return "T_SEMICOLON";
    if (type == COLON) return "T_COLON";
    if (type == SLASH) return "T_SLASH";
    if (type == STAR) return "T_STAR";
    if (type == ASSIGN) return "T_ASSIGN";
    if (type == BANG) return "T_BANG";
    if (type == BANG_EQUAL) return "T_BANG_EQUAL";
    if (type == EQUAL) return "T_EQUAL";
    if (type == GREATER) return "T_GREATER";
    if (type == GREATER_EQUAL) return "T_GREATER_EQUAL";
    if (type == LESS) return "T_LESS";
    if (type == LESS_EQUAL) return "T_LESS_EQUAL";

    if (type == AND) return "T_AND";
    if (type == OR) return "T_OR";

    if (type == IDENTIFIER) return "T_IDENTIFIER";
    if (type == STRING) return "T_STRING";
    if (type == INTEGER) return "T_INTEGER";
    if (type == FLOAT) return "T_FLOAT";

    if (type == IF) return "T_IF";
    if (type == ELSE) return "T_ELSE";
    if (type == TRUE) return "T_TRUE";
    if (type == FALSE) return "T_FALSE";
    if (type == NIL) return "T_NIL";
    if (type == FN) return "T_FN";
    if (type == RETURN) return "T_RETURN";
    if (type == FOR) return "T_FOR";
    if (type == WHILE) return "T_WHILE";
    if (type == LET) return "T_LET";
    if (type == CONST) return "T_CONST";
    return NULL;
}

Tokenizer_Error eat_token(enum TokenType type)
{
    if (current->type == type) {
        current = current->next;
        return (Tokenizer_Error) {
            .error = NULL
        };
    }

    char *buffer = malloc(100);

    sprintf(
        buffer, 
        "Unexptected token, expected '%s', got '%s'",
        token_type_to_str(type),
        token_type_to_str(current->type)
    );

    return (Tokenizer_Error) {
        .error = buffer
    };
}

void reset_stream()
{
    current = token_head;
}

Token *current_token()
{
    return current;
}

Token *next_token()
{
    return current->next;
}

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

Tokenizer_Error tokenize(FILE *input)
{
    int line = 1;
    int line_position = 1;
    char c;
    while((c = fgetc(input)) != EOF) {
        if (isspace(c)) continue;

        Token_Metadata *meta = malloc(sizeof(Token_Metadata));
        meta->line = line;
        meta->length = 1;
        meta->line_position = line_position;
        switch (c) {
            case '(': {push_token(meta, LEFT_PAREN, NULL);} break;
            case ')': {push_token(meta, RIGHT_PAREN, NULL);} break;
            case '{': {push_token(meta, LEFT_BRACE, NULL);} break;
            case '}': {push_token(meta, RIGHT_BRACE, NULL);} break;
            case '+': {push_token(meta, PLUS, NULL);} break;
            case '-': {push_token(meta, MINUS, NULL);} break;
            case '*': {push_token(meta, STAR, NULL);} break;
            case '.': {push_token(meta, DOT, NULL);} break;
            case ',': {push_token(meta, COMMA, NULL);} break;
            case ';': {push_token(meta, SEMICOLON, NULL);} break;
            case '/': {
                char tmp = ts_peek(input);
                if (tmp == '/') {
                    fgetc(input); // this case of single line comment, just remove every thing up to new line. Just like this one :D
                    for(;;) {
                        tmp = fgetc(input);
                        if (tmp == '\n') break;
                    }
                } else {
                    push_token(meta, SLASH, NULL);
                }
            } break;
            case ':': {
                char tmp = ts_peek(input);
                if (tmp == '=') { meta->length = 2; push_token(meta, ASSIGN, NULL); fgetc(input); }
                else { push_token(meta, COLON, NULL); }
            } break;
            case '!': {
                char tmp = ts_peek(input);
                if (tmp == '='){ meta->length = 2; push_token(meta, BANG_EQUAL, NULL); fgetc(input); }
                else { push_token(meta, BANG, NULL); }
            } break;
            case '=': push_token(meta, EQUAL, NULL); break;
            case '>': {
                char tmp = ts_peek(input);
                if (tmp == '=') { meta->length = 2; push_token(meta, GREATER_EQUAL, NULL); fgetc(input); }
                else { push_token(meta, GREATER, NULL); }
            } break;
            case '<': {
                char tmp = ts_peek(input);
                if (tmp == '=') { meta->length = 2; push_token(meta, LESS_EQUAL, NULL); fgetc(input); }
                else { push_token(meta, LESS, NULL); }
            } break;
            case '"': {
                char tmp;
                size_t buffer_size = 1024;
                char *buffer = malloc(buffer_size);
                size_t count = 0;
                for(;;) {
                    tmp = ts_peek(input);
                    if (isalphanum(tmp) || isspace(tmp)) {
                        if (count >= buffer_size) {
                            buffer_size = buffer_size + 1024;
                            buffer = realloc(buffer, buffer_size);
                        }
                        buffer[count++] = tmp;
                        fgetc(input);
                    }
                    if (tmp == '\n') {
                        fgetc(input);
                        ++line;
                        line_position = 1;
                        continue;
                    }
                    if (tmp == '"') {
                        fgetc(input);
                        buffer[count] = '\0';
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

        //read identifier
        if (isalpha(c)) {
            size_t buffer_size = 1024;
            char *buffer = malloc(buffer_size);
            size_t count = 0;
            buffer[count++] = c;
            for(;;) {
                char tmp = ts_peek(input);
                if (isalpha(tmp)) {
                    if (count >= buffer_size) {
                        buffer_size = buffer_size + 1024;
                        buffer = realloc(buffer, buffer_size);
                    }
                    buffer[count++] = tmp;
                    fgetc(input);
                }
                
                if (isspace(tmp)) {
                    buffer[count] = '\0';
                    break;
                }
            }
            meta->length = count;
            push_token(meta, IDENTIFIER, buffer);
        }

        // read number
        if (isdigit(c)) {
            char tmp;
            enum TokenType type = INTEGER;
            size_t count = 0;
            size_t buffer_size = 1024;
            char *buffer = malloc(buffer_size);
            buffer[count++] = c;
            for(;;) {
                tmp = ts_peek(input);
                if (isdigit(tmp) || tmp == '.') {
                    buffer[count++] = tmp;
                    if (tmp == '.') type = FLOAT;
                    fgetc(input);
                } else {
                    break;
                }
            }
            buffer[count] = '\0';
            meta->length = count;
            push_token(meta, type, buffer);
            continue;
        }

        ++line_position;
    }

    push_token(NULL, EOF, NULL);

    reset_stream();
    return (Tokenizer_Error) {
        .error = NULL
    };
}

Tokenizer_Error token_stream_shutdown()
{  
    Token *tmp;
    while (token_head != NULL) {
        tmp = token_head->next;
        if (token_head->allocated == 1) { free(token_head->value); }
        free(token_head->meta_data);
        token_head = tmp;
    }

    return (Tokenizer_Error) {
        .error = NULL
    };
}
