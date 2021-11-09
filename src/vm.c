#include "vm.h"
#include <stdio.h>
#include <string.h>

bool is_truthy(Value *val)
{
    if (val->type == AV_BOOL) {
        return val->b ?: false;
    }
    if (val->type == AV_STRING) {
        return val->str->size > 0;
    }
    if (val->type == AV_NUMBER) {
        return val->n != 0;
    }
    if (val->type == AV_NIL) {
        return false;
    }

    return false;
}

bool is_equal(Value *v1, Value *v2)
{
    if (v1->type != v2->type) {
        return false;
    }

    if (v1->type == AV_BOOL) {
        return v1->b == v2->b;
    }

    if (v1->type == AV_STRING) {
        return string_equal(v1->str, v2->str);
    }
    
    if (v1->type == AV_NUMBER) {
        return v1->n == v2->n;
    }

    if (v1->type == AV_NIL){
        return true;
    }

    return false;
}


void cleanup(Lexer *lexer)
{
    Token *token = lexer->tokens;

    free((void*)lexer->file->content);
    while(token) {
        Token *tmp = token->next;
        free(token);
        token = tmp;
    }
}

bool string_equal(const String *s1, const String *s2)
{
   if (s1->size != s2->size) {
       return false;
   }

   const char *s1data = s1->text;
   const char *s2data = s2->text;

   for(size_t i = 0; i < s1->size; i++) {
       if (*(s1data++) != *(s2data++)) {
           return false;
       }
   }

   return true;
}

bool line_in_token_range(Token *token, int current_line)
{
    int range = 5;
    int token_line = token->location.line;
    int upper = token_line + range;
    int lowwer = token_line - range;
    return (lowwer <= current_line && current_line <= upper);
}

void print_token_in_file(Token *token)
{
    Lexer *lexer = token->lexer;
    SourceCodeFile *file = lexer->file;
    int line_size = 0;
    const char *line;
    const char **lines = file->lines;
    int upper = token->location.line - 5;
    if (upper <= 0) {
        upper = 1;
    }
    int lower = token->location.line + 5;
    
    while(true) {
        line = lines[upper];
        if (line == NULL || strcmp(line, "") == 0) break;
        for(const char* tmp = line; (int)*tmp != '\n'; line_size++, tmp++);
        if(upper == token->location.line) {
            fprintf(stdout, "-> %d | %.*s\n", upper, line_size, line);
            int position = token->location.column + 7;
            while(position--) fprintf(stdout, " ");
            fprintf(stdout, "^\n");
        } else {
            fprintf(stdout, "   %d | %.*s\n", upper, line_size, line);
        }
        line_size = 0;
        upper++;
        if (upper >= lower) break;
    }

    free(lines);
}


void print_compiler_error(Token *token, const char *message)
{
    FileLocation location = token->location;
    fprintf(stdout, "\n\033[1;0m%s:%d:%d: \033[1;31merror: \033[0m%s\n", location.file_name, location.line, location.column, message);
    print_token_in_file(token);
    cleanup(token->lexer);
}

void print_compiler_note(Token *token, const char *message)
{
    FileLocation location = token->location;
    fprintf(stdout, "\033[1;0m%s:%d:%d: note: \033[0m%s\n\n", location.file_name, location.line, location.column, message);
    
}
