#include "parser.h"
#include "tokenizer.h"
#include "ast.h"

void eat_token(Lexer *lexer, enum TokenType type)
{
    if (lexer->current_token->type == type) {
        lexer->current_token = lexer->current_token->next;
        return;
    }

    char *buffer = malloc(1024);

    sprintf(
        buffer, 
        "Unexptected token, expected '%s', got '%s'",
        token_type_to_str(type),
        token_type_to_str(lexer->current_token->type)
    );

    print_compiler_error(lexer->current_token, buffer);
    free(buffer);
    exit(EXIT_FAILURE);
}

Ast *program(Lexer *lexer)
{
    Ast *stmt_list = statement_list(lexer);
    Token *token = lexer->current_token;
    if (token->type == T_EOF) {
        return stmt_list;
    }

    char *err_buf = malloc(512);
    sprintf(
        err_buf, 
        "Unexpected token at end of lexer, expected 'EOF', got: '%s'(%.*s)", 
        token_type_to_str(token->type),
        (int)token->value.size,
        token->value.text
    );
    print_compiler_error(token, err_buf);
    free(err_buf);
    exit(EXIT_FAILURE);
}

bool dont_need_semicolon(enum AstType type)
{
    return type == AT_FN_DEFINITION || 
        type == AT_IF ||
        type == AT_WHILE ||
        type == AT_FOR;
}

Ast *statement_list(Lexer *lexer)
{
    Ast *stmt = statement(lexer);
    Ast *stmt_list = make_statement_list(lexer->current_token, NULL, stmt);
    Token *token = lexer->current_token;
    bool run = dont_need_semicolon(stmt->type);
    while(token->type == T_SEMICOLON || run) {
        if (!run) {
            eat_token(lexer, T_SEMICOLON);
        }
        run = false;
        Ast *stmt = statement(lexer);
        if (dont_need_semicolon(stmt->type)) {
            run = true;
        }
        stmt_list = make_statement_list(token, stmt_list, stmt);
        token = lexer->current_token;
    }

    return stmt_list;
}

Ast *_while(Lexer *lexer)
{
    Token *token = lexer->current_token;
    eat_token(lexer, T_WHILE);
    return make_while(token, or(lexer), compound_statement(lexer));
}

Ast *_for(Lexer *lexer)
{
    Token *token = lexer->current_token;
    eat_token(lexer, T_FOR);
    return make_for(token, or(lexer), compound_statement(lexer));
}

Ast *_if(Lexer *lexer)
{
    Token *token = lexer->current_token;
    eat_token(lexer, T_IF);

    return make_if(token, or(lexer), compound_statement(lexer));
}

Ast *function_call_expr(Lexer *lexer)
{
    Token *token = lexer->current_token;
    eat_token(lexer, T_IDENTIFIER);
    return make_function_call(token, expr_list(lexer));
}

Ast *function_definition_statement(Lexer *lexer)
{
    eat_token(lexer, T_FN);
    Token *token = lexer->current_token;
    eat_token(lexer, T_IDENTIFIER);

    return make_function_definition(
        token, 
        expr_list(lexer),
        compound_statement(lexer)
    );
}

Ast *statement(Lexer *lexer)
{
    Token *token = lexer->current_token;
    if (token->type == T_LBRACE) {
        return compound_statement(lexer);
    }

    if (token->type == T_IDENTIFIER && token->next->type == T_LPAREN) {
        return function_call_expr(lexer);
    }

    if (token->type == T_FN && token->next->type == T_IDENTIFIER) {
        return function_definition_statement(lexer);
    }

    if (token->type == T_CONST || token->type == T_IDENTIFIER) {
        return assigment(lexer);
    }

    if (token->type == T_IF) {
        return _if(lexer);
    }

    if (token->type == T_WHILE) {
        return _while(lexer);
    }

    if (token->type == T_RETURN) {
        eat_token(lexer, T_RETURN);
        return make_return(token, or(lexer));
    }

    if (token->type == T_BREAK) {
        eat_token(lexer, T_BREAK);
        return make_keyword(token);    
    }

    if (token->type == T_CONTINUE) {
        eat_token(lexer, T_CONTINUE);
        return make_keyword(token);
    }

    return empty(lexer);
}

Ast *assigment(Lexer *lexer)
{
    Ast *var = variable(lexer);
    Token *token = lexer->current_token;
    eat_token(lexer, T_ASSIGN);
    return make_assigment(token, var, or(lexer));
}

Ast *compound_statement(Lexer *lexer)
{
    eat_token(lexer, T_LBRACE);
    Ast *list = statement_list(lexer);
    eat_token(lexer, T_RBRACE);
    return list;
}

Ast *variable(Lexer *lexer)
{
    Token *token = lexer->current_token;
    bool is_const = false;
    if (token->type == T_CONST) {
        eat_token(lexer, T_CONST);
        is_const = true;
        token = lexer->current_token;
    }

    eat_token(lexer, T_IDENTIFIER);
    return make_variable(token, is_const);
}

Ast *empty(Lexer *lexer)
{
    return make_empty(lexer->current_token);
}

Ast *factor(Lexer *lexer)
{
    Token *token = lexer->current_token;

    if (token->type == T_IDENTIFIER && token->next->type == T_LPAREN) {
        return function_call_expr(lexer);
    }

    if (token->type == T_PLUS) {
        eat_token(lexer, T_PLUS);
        return factor(lexer);
    }

    if (token->type == T_MINUS) {
        eat_token(lexer, T_MINUS);
        return make_unary(token, factor(lexer));
    }

    if (token->type == T_NUMBER) {
        char *end;
        Value val = (Value) {
            .type = AV_NUMBER,
            .n = strtof(token->value.text, &end)
        };
        eat_token(lexer, T_NUMBER);
        return make_value(token, &val);
    }

    if (token->type == T_STRING) {
        Value val = (Value) {
            .type = AV_STRING,
            .str = &token->value
        };
        eat_token(lexer, T_STRING);
        return make_value(token, &val);
    }

    if (token->type == T_TRUE || token->type == T_FALSE || token->type == T_NIL) {
        if (token->type == T_NIL) {
            eat_token(lexer, T_NIL);
            return make_value(token, NULL);
        }
        Value val = (Value) {
            .type = AV_BOOL
        };
        if (token->type == T_TRUE) {
            eat_token(lexer, T_TRUE);
            val.b = true;
        } else {
            eat_token(lexer, T_FALSE);
            val.b = false;
        }

        return make_value(token, &val);
    }

    if (token->type == T_LPAREN) {
        eat_token(lexer, T_LPAREN);
        Ast *in_expr = or(lexer);
        eat_token(lexer, T_RPAREN);
        return in_expr;
    }

    return variable(lexer);
}

Ast *term(Lexer *lexer) {
    Ast *lvalue = factor(lexer);

    Token *token = lexer->current_token;
    while(token->type == T_SLASH || token->type == T_STAR) {
        eat_token(lexer, token->type);
        lvalue = make_binop(token, lvalue, factor(lexer));
        token = lexer->current_token;
    }

    return lvalue;
}

Ast *expr_list(Lexer *lexer)
{
    eat_token(lexer, T_LPAREN);
    Token *token = lexer->current_token;
    if (token->type == T_RPAREN) {
        eat_token(lexer, T_RPAREN);
        return empty(lexer);
    }

    Token *first = token;
    Ast *expression = or(lexer);
    expression->next = NULL;
    Ast *tmp = expression;
    token = lexer->current_token;
    while (token->type == T_COMMA) {
        eat_token(lexer, T_COMMA);
        tmp->next = or(lexer); 
        tmp = tmp->next;
        token = lexer->current_token;
    }

    eat_token(lexer, T_RPAREN);
    return make_expr_list(first, expression);
}

Ast *expr(Lexer *lexer)
{
    Ast *lvalue = term(lexer);
    Token *token = lexer->current_token;

    while (token->type == T_PLUS || token->type == T_MINUS) {
        eat_token(lexer, token->type);
        lvalue = make_binop(token, lvalue, term(lexer));
        token = lexer->current_token;
    }

    return lvalue;
}

Ast *and(Lexer *lexer) 
{
    Ast *lvalue = equal(lexer);
    Token *token = lexer->current_token;

    while(token->type == T_AND) {
        eat_token(lexer, T_AND);
        lvalue = make_logical(token, lvalue, equal(lexer));
        token = lexer->current_token;
    }

    return lvalue;
}

Ast *or(Lexer *lexer)
{
    Ast *lvalue = and(lexer);
    Token *token = lexer->current_token;

    while(token->type == T_OR) {
        eat_token(lexer, T_OR);
        lvalue = make_logical(token, lvalue, and(lexer));
        token = lexer->current_token;
    }

    return lvalue;
}

bool is_cmp_token(Token *token)
{
    enum TokenType cmp_types[] = {T_CMP, T_LESS, T_LESS_EQUAL, T_GREATER, T_GREATER_EQUAL};
    for(size_t i=0; i<(sizeof(cmp_types)/sizeof(enum TokenType)); i++) {
        if (cmp_types[i] == token->type) return true;
    }
    return false;
}

Ast *equal(Lexer *lexer) 
{
    Ast *lvalue = expr(lexer);
    Token *token = lexer->current_token;
    Token *current = token;
    while(is_cmp_token(token)) {
        if (token->type != current->type) break;
        eat_token(lexer, token->type);
        lvalue = make_logical(token, lvalue, expr(lexer));
        token = lexer->current_token;
    }

    return lvalue;
}
