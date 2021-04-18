#include "token.h"

Token *token_head;
Token *current_token_stream;

Token *keywords;
Token *current_keyword;

void make_keyword(enum TokenType type, char *val)
{
    Token *keyword = malloc(sizeof(Token));
    keyword->type = type;
    size_t word_length = strlen(val);
    keyword->value = malloc(word_length);
    strncpy(keyword->value, val, word_length);
    keyword->allocated = 1;
    keyword->next = NULL;

    if (keywords == NULL) {
        keywords = keyword;
        current_keyword = keywords;
    } else {
        current_keyword->next = keyword;
        current_keyword = current_keyword->next;
    }
}

void create_keywords()
{
    make_keyword(T_CONST, "const");
    make_keyword(T_IF, "if");
    make_keyword(T_ELSE, "else");
    make_keyword(T_TRUE, "true");
    make_keyword(T_FALSE, "false");
    make_keyword(T_NIL, "nil");
    make_keyword(T_FOR, "for");
    make_keyword(T_WHILE, "while");
    make_keyword(T_FN, "fn");
    make_keyword(T_STRUCT, "struct");
}

Token *get_keyword(char *identifier)
{
    Token *tmp = keywords;
    while (tmp != NULL) {
        if (strcmp(tmp->value, identifier) == 0) return tmp;
        tmp = tmp->next;
    }
    
    return NULL;
}

char *token_type_to_str(enum TokenType type) {
    if (type == T_PLUS) return "T_PLUS";
    if (type == T_MINUS) return "T_MINUS";
    if (type == T_LPAREN) return "T_LPAREN";
    if (type == T_RPAREN) return "T_RPAREN";
    if (type == T_LBRACE) return "T_LBRACE";
    if (type == T_RBRACE) return "T_RBRACE";
    if (type == T_COMMA) return "T_COMMA";
    if (type == T_DOT) return "T_DOT";
    if (type == T_SEMICOLON) return "T_SEMICOLON";
    if (type == T_COLON) return "T_COLON";
    if (type == T_SLASH) return "T_SLASH";
    if (type == T_STAR) return "T_STAR";
    if (type == T_ASSIGN) return "T_ASSIGN";
    if (type == T_BRANG) return "T_BANG";
    if (type == T_BANG_EQUAL) return "T_BANG_EQUAL";
    if (type == T_EQUAL) return "T_EQUAL";
    if (type == T_GREATER) return "T_GREATER";
    if (type == T_GREATER_EQUAL) return "T_GREATER_EQUAL";
    if (type == T_LESS) return "T_LESS";
    if (type == T_LESS_EQUAL) return "T_LESS_EQUAL";
    if (type == T_AND) return "T_AND";
    if (type == T_OR) return "T_OR";
    if (type == T_IDENTIFIER) return "T_IDENTIFIER";
    if (type == T_STRING) return "T_STRING";
    if (type == T_INTEGER) return "T_INTEGER";
    if (type == T_FLOAT) return "T_FLOAT";
    if (type == T_CONST) return "T_CONST";
    if (type == T_EOF) return "T_EOF";
    if (type == T_FN) return "T_FN";
    if (type == T_FOR) return "T_FOR";
    if (type == T_WHILE) return "T_WHILE";
    if (type == T_IF) return "T_IF";
    if (type == T_ELSE) return "T_ELSE";
    if (type == T_TRUE) return "T_TRUE";
    if (type == T_FALSE) return "T_FASLE";
    if (type == T_NIL) return "T_NIL";
    return NULL;
}

Tokenizer_Error eat_token(enum TokenType type)
{
    if (current_token_stream->type == type) {
        current_token_stream = current_token_stream->next;
        return (Tokenizer_Error) {
            .error = NULL
        };
    }

    char *buffer = malloc(100);

    sprintf(
        buffer, 
        "Unexptected token, expected '%s', got '%s'",
        token_type_to_str(type),
        token_type_to_str(current_token_stream->type)
    );

    return (Tokenizer_Error) {
        .error = buffer
    };
}

void reset_stream()
{
    current_token_stream = token_head;
}

Token *current_token()
{
    return current_token_stream;
}

void token_stream_print()
{
    Token *token = token_head;
    while(token != NULL) {
        fprintf(stdout, "%s(%s) ", token_type_to_str(token->type), token->value);
        token = token->next;
    }
    fprintf(stdout, "\n");
}

Token *next_token()
{
    return current_token_stream->next;
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
    size_t val_length = strlen(value);
    t->value = malloc(sizeof(val_length));
    strncpy(t->value, value, val_length);
    t->allocated = 1;
    
    if (token_head == NULL) {
        token_head = t;
        current_token_stream = token_head;
    } else {
        current_token_stream->next = t;
        current_token_stream = current_token_stream->next;
    }
}

Tokenizer_Error tokenize(FILE *input)
{
    create_keywords();
    int line = 1;
    int line_position = 1;
    char c;
    while((c = fgetc(input)) != EOF) {
        if (isspace(c)) continue;
        Token_Metadata *meta = malloc(sizeof(Token_Metadata));
        meta->line = line;
        meta->length = 1;
        meta->line_position = line_position;
        // read number
        if (isdigit(c)) {
            char tmp;
            enum TokenType type = T_INTEGER;
            size_t count = 0;
            size_t buffer_size = 1024;
            char *buffer = malloc(buffer_size);
            buffer[count] = c;
            ++count;
            for(;;) {
                tmp = ts_peek(input);
                if (isdigit(tmp) || tmp == '.') {
                    buffer[count] = tmp;
                    ++count;
                    if (tmp == '.') type = T_FLOAT;
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

        //read identifier
        if (isalpha(c)) {
            size_t buffer_size = 1024;
            char *buffer = malloc(buffer_size);
            size_t count = 0;
            buffer[count] = c;
            ++count;
            for(;;) {
                char tmp = ts_peek(input);
                if (isalpha(tmp) || tmp == '_') {
                    if (count >= buffer_size) {
                        buffer_size = buffer_size + 1024;
                        buffer = realloc(buffer, buffer_size);
                    }
                    buffer[count] = tmp;
                    ++count;
                    fgetc(input);
                    continue;
                }
                
                if (isspace(tmp)) {
                    buffer[count] = '\0';
                    break;
                }
            }
            meta->length = count;
            Token *keyword = get_keyword(buffer);
            if (keyword) {
                push_token(meta, keyword->type, keyword->value);
            } else {
                push_token(meta, T_IDENTIFIER, buffer);
            }

            continue;
        }

        switch (c) {
            case '(': {push_token(meta, T_LPAREN, "(");} break;
            case ')': {push_token(meta, T_RPAREN, ")");} break;
            case '{': {push_token(meta, T_LBRACE, "{");} break;
            case '}': {push_token(meta, T_RBRACE, "}");} break;
            case '+': {push_token(meta, T_PLUS, "+");} break;
            case '-': {push_token(meta, T_MINUS, "-");} break;
            case '*': {push_token(meta, T_STAR, "*");} break;
            case '.': {push_token(meta, T_DOT, ".");} break;
            case ',': {push_token(meta, T_COMMA, ",");} break;
            case ';': {push_token(meta, T_SEMICOLON, ";");} break;
            case '&': {
                char tmp = ts_peek(input);
                if (tmp == '&') {
                    fgetc(input);
                    push_token(meta, T_AND, "&&");
                }
            } break;
            case '|': {
                char tmp = ts_peek(input);
                if (tmp == '|') {
                    fgetc(input);
                    push_token(meta, T_OR, "||");
                }
            } break;
            case '/': {
                char tmp = ts_peek(input);
                if (tmp == '/') {
                    fgetc(input); // this case of single line comment, just remove every thing up to new line. Just like this one :D
                    for(;;) {
                        tmp = fgetc(input);
                        if (tmp == '\n') break;
                    }
                } else {
                    push_token(meta, T_SLASH, "/");
                }
            } break;
            case ':': {
                char tmp = ts_peek(input);
                if (tmp == '=') { meta->length = 2; push_token(meta, T_ASSIGN, ":="); fgetc(input); }
                else { push_token(meta, T_COLON, ":"); }
            } break;
            case '!': {
                char tmp = ts_peek(input);
                if (tmp == '='){ meta->length = 2; push_token(meta, T_BANG_EQUAL, "!="); fgetc(input); }
                else { push_token(meta, T_BRANG, "!"); }
            } break;
            case '=': push_token(meta, T_EQUAL, "="); break;
            case '>': {
                char tmp = ts_peek(input);
                if (tmp == '=') { meta->length = 2; push_token(meta, T_GREATER_EQUAL, ">="); fgetc(input); }
                else { push_token(meta, T_GREATER, ">"); }
            } break;
            case '<': {
                char tmp = ts_peek(input);
                if (tmp == '=') { meta->length = 2; push_token(meta, T_LESS_EQUAL, "<="); fgetc(input); }
                else { push_token(meta, T_LESS, "<"); }
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
                        buffer[count] = tmp;
                        ++count;
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
                push_token(meta, T_STRING, buffer);
            } break;
            case '\n': {
                ++line;
                line_position = 1;
            } break;
            default: {} break;
        }

        ++line_position;
    }

    push_token(NULL, T_EOF, "EOF");
    reset_stream();

    return (Tokenizer_Error) {
        .error = NULL
    };
}

Tokenizer_Error token_stream_shutdown()
{  
    Token *node = token_head;
    while (node != NULL) {
        Token *tmp = node;
        node = node->next;
        if (node->allocated == 1) { free(node->value); }
        free(node->meta_data);
        free(tmp);
    }
    token_head = NULL;

    node = keywords;
    while(node != NULL)
    {
        Token *tmp = node;
        node = node->next;
        if (keywords->allocated == 1) {free(keywords->value); }
        free(tmp);
    }
    keywords = NULL;

    reset_stream();
    return (Tokenizer_Error) {
        .error = NULL
    };
}