#include "tokenizer.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "vm.h"

void debug_tokenize(Lexer *lexer)
{
    Token * token = lexer->tokens;
    int current_line = 1;
    while (token) {
        int line = token->location.line;
        if (current_line != line) {
            printf("\n");
            current_line = line;
        }
        if (token->value.text != NULL) {
            print_token(token);
        }
        else {
            printf("%s", token_type_to_str(token->type));
        }
        printf(" ");
        token = token->next;
    }
}

TokenKeyword keywords[] = {
    {T_CONST, {5, "const"}},
    {T_IF, {2, "if"}},
    {T_ELSE, {4, "else"}},
    {T_TRUE, {4, "true"}},
    {T_FALSE, {5, "false"}},
    {T_NIL, {3, "nil"}},
    {T_FOR, {3, "for"}},
    {T_WHILE, {5, "while"}},
    {T_FN, {2, "fn"}},
    {T_STRUCT, {6, "struct"}},
    {T_RETURN, {6, "return"}},
    {T_BREAK, {5, "break"}},
    {T_CONTINUE, {8, "continue"}}
};

TokenKeyword *get_keyword(const String *keyword)
{    
    size_t keyword_size = sizeof(keywords)/sizeof(TokenKeyword);
    for(size_t i = 0; i < keyword_size; i++)
    {
        if (string_equal(&keywords[i].value, keyword)) {
            return &keywords[i];
        }
    } 
    return NULL;
}

const char *token_type_to_str(enum TokenType type) {
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
    if (type == T_BANG) return "T_BANG";
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
    if (type == T_NUMBER) return "T_NUMBER";
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
    if (type == T_BREAK) return "T_BREAK";
    if (type == T_CONTINUE) return "T_CONTINUE";
    return NULL;
}

void print_token(Token *token)
{
    printf("%s(%.*s)", token_type_to_str(token->type), (int)token->value.size, token->value.text);
}

int isalphanum(char c)
{
    return isdigit(c) || isalpha(c);
}

void push_token(Lexer *lexer, enum TokenType type, const char *value, size_t size)
{
    Token *t = malloc(sizeof(Token));
    t->type = type;
    t->lexer = lexer;
    t->next = NULL;

    t->value = (String) {
        .text = value,
        .size = size
    };

    size_t column = lexer->file->column - size;

    if (column <= 0) {
        column = 1;
    }

     t->location = (FileLocation) {
         .file_name = lexer->file->file_name,
         .line = lexer->file->line,
         .column = column,
     };

    Token *token = lexer->tokens;
    if (!token) {
        lexer->tokens = t;
        lexer->current_token = t;
    } else {
        while(token->next != NULL) token = token->next;
        token->next = t;
    }
}

const char *token_start(Lexer *lexer)
{
    return &lexer->file->content[lexer->current];
}

char lexer_getc(Lexer *lexer)
{
    return lexer->file->content[lexer->current];
}

char lexer_peek(Lexer *lexer)
{
    return lexer->file->content[lexer->current+1];
}

void advance_lexer(Lexer *lexer)
{
    if (lexer_getc(lexer) == '\n') {
        lexer->file->line++;
        lexer->file->column = 1;
    }
    else {
        lexer->file->column++;
    }
    lexer->current = lexer->current+1;
}

void read_string(Lexer *lexer)
{
    size_t count = 0;
    advance_lexer(lexer);
    const char *start = token_start(lexer);
    while(lexer_getc(lexer) != '"') { 
        count++; 
        advance_lexer(lexer);
    }
    advance_lexer(lexer);
    
    push_token(lexer, T_STRING, start, count);
}

void read_number(Lexer *lexer)
{
    char c;
    enum TokenType type = T_NUMBER;
    size_t count = 0;
    const char *start = token_start(lexer);
    while(true) {
        c = lexer_getc(lexer);
        if (isdigit(c) || c == '.') {
            count++;
            advance_lexer(lexer);
        } else break;
    }
    push_token(lexer, type, start, count);
}

void read_identifier(Lexer *lexer)
{
    enum TokenType type = T_IDENTIFIER;
    size_t count = 0;
    const char *start = token_start(lexer);
    for(;;) {
        char tmp = lexer_getc(lexer);
        if (isalphanum(tmp) || tmp == '_') {
            count = count + 1;
            advance_lexer(lexer);
            continue;
        } else break;
    }

    String keyword_buffer = (String) {
        .size = count,
        .text = start 
    };

    TokenKeyword *keyword = get_keyword(&keyword_buffer);
    if (keyword) {
        type = keyword->token_type;
    }
    push_token(lexer, type, start, count);
}

void read_punctuation(Lexer *lexer)
{
    char c = lexer_getc(lexer);
    switch (c) {
        case '@': {push_token(lexer, T_AT, NULL, 1);} break;
        case '%': {push_token(lexer, T_PERCENT, NULL, 1);} break;
        case '(': {push_token(lexer, T_LPAREN, NULL, 1);} break;
        case ')': {push_token(lexer, T_RPAREN, NULL, 1);} break;
        case '{': {push_token(lexer, T_LBRACE, NULL, 1);} break;
        case '}': {push_token(lexer, T_RBRACE, NULL, 1);} break;
        case '+': {push_token(lexer, T_PLUS, NULL, 1);} break;
        case '-': {push_token(lexer, T_MINUS, NULL, 1);} break;
        case '*': {push_token(lexer, T_STAR, NULL, 1);} break;
        case '.': {push_token(lexer, T_DOT, NULL, 1);} break;
        case ',': {push_token(lexer, T_COMMA, NULL, 1);} break;
        case ';': {push_token(lexer, T_SEMICOLON, NULL, 1);} break;
        case '/': {push_token(lexer, T_SLASH, NULL, 1);} break;
        case '=': {
             char tmp = lexer_peek(lexer);
             if (tmp == '=') {
                 advance_lexer(lexer);
                 push_token(lexer, T_CMP, NULL, 2);
             } else {
                 push_token(lexer, T_EQUAL, NULL, 1);
             }
        } break;
        case '&': {
            char tmp = lexer_peek(lexer);
            if (tmp == '&') {
                advance_lexer(lexer);
                push_token(lexer, T_AND, NULL, 2);
            } else {
                push_token(lexer, T_BITWISE_AND, NULL, 1);
            }
        } break;
        case '|': {
            char tmp = lexer_peek(lexer);
            if (tmp == '|') {
                advance_lexer(lexer);
                push_token(lexer, T_OR, NULL, 2);
            } else {
                push_token(lexer, T_BITWISE_OR, NULL, 1);
            }
        } break;
        case ':': {
            char tmp = lexer_peek(lexer);
            if (tmp == '=') { push_token(lexer, T_ASSIGN, NULL, 2); advance_lexer(lexer); }
            else { push_token(lexer, T_COLON, NULL, 1); }
        } break;
        case '!': {
            char tmp = lexer_peek(lexer);
            if (tmp == '='){ push_token(lexer, T_BANG_EQUAL, NULL, 2); advance_lexer(lexer); }
            else { push_token(lexer, T_BANG, NULL, 1); }
        } break;
        case '>': {
            char tmp = lexer_peek(lexer);
            if (tmp == '=') { push_token(lexer, T_GREATER_EQUAL, NULL, 2); advance_lexer(lexer); }
            else { push_token(lexer, T_GREATER, NULL, 1); }
        } break;
        case '<': {
            char tmp = lexer_peek(lexer);
            if (tmp == '=') { 
                push_token(lexer, T_LESS_EQUAL, NULL, 2); 
                advance_lexer(lexer);
            } else {
                push_token(lexer, T_LESS, NULL, 1); 
            }
        } break;
        default: break;
    }
    advance_lexer(lexer);
}

void skip_comment(Lexer *lexer)
{
    char c;
    while((c = lexer_getc(lexer)) != '\n') advance_lexer(lexer);
    advance_lexer(lexer);
}

void tokenize(Lexer *lexer)
{
    char c;
    while((c = lexer_getc(lexer)) != EOF) {
        if (isspace(c)) {
            advance_lexer(lexer);
            continue;
        }
        // read number
        if (isdigit(c)) {
            read_number(lexer);
            continue;
        }

        if (c == '/' && lexer_peek(lexer) == '/') {
            skip_comment(lexer);
            continue;
        }

        //read identifier
        if (isalpha(c) || c == '_') {
            read_identifier(lexer);
            continue;
        }

        if (c == '"') {
            read_string(lexer);
            continue;
        }

        read_punctuation(lexer);

        if (c == 0) {
            break;
        }
    }

    push_token(lexer, T_EOF, NULL, 0);
}
