#include "parser.h"

Ast *program()
{
    Ast *stmt_list = statement_list();
    Token *token = current_token();
    if (token->type != T_EOF) {
        char *err_buf = malloc(200);
        sprintf(err_buf, "Unexpected token at end of file, expected 'EOF', got: '%s'(%s)", token_type_to_str(token->type), token->value);
        return make_error(err_buf);
    }

    return stmt_list;
}

Ast *statement_list()
{
    Ast *stmt = statement();
    Ast *stmt_list = make_statement_list(NULL, stmt);
    Token *token = current_token();
    while(token->type == T_SEMICOLON) {
        EAT(T_SEMICOLON);
        Ast *in_stmt = statement();
        ASSERT_ERROR(in_stmt);
        stmt_list = make_statement_list(stmt_list, in_stmt);
        token = current_token();
    }

    return stmt_list;
}

Ast *statement()
{
    Token *token = current_token();

    if (token->type == T_LBRACE) {
        Ast *statement = compound_statement();
        ASSERT_ERROR(statement);
        return statement;
    }

    if (token->type == T_CONST || token->type == T_IDENTIFIER) 
    {
        Ast *statement = assigment_statement();
        ASSERT_ERROR(statement);
        return statement;
    }

    return empty();
}

Ast *assigment_statement()
{
    Ast *var = variable();
    ASSERT_ERROR(var);
    EAT(T_ASSIGN);
    Ast *expression = expr();
    ASSERT_ERROR(expression);
    return make_assigment(var, expression);
}

Ast *compound_statement()
{
    EAT(T_LBRACE);
    Ast *list = statement_list();
    EAT(T_RBRACE);
    return list;
}

Ast *variable()
{
    Token *token = current_token();
    int is_const = 0;
    if (token->type == T_CONST) {
        EAT(T_CONST);
        is_const = 1;
    }
    token = current_token();

    EAT(T_IDENTIFIER);
    return make_variable(token->value, is_const);
}

Ast *empty()
{
    return make_empty();
}


Ast *factor()
{
    Token *token = current_token();
    if (token == NULL) {
        return make_error("Unexpected token, token is null!");
    }
    
    if (token->type == T_PLUS) {
        EAT(T_PLUS);
        Ast *value = factor();
        ASSERT_ERROR(value);
        return make_unary(AU_PLUS, value);
    }

    if (token->type == T_MINUS) {
        EAT(T_MINUS);
        Ast *value = factor();
        ASSERT_ERROR(value);
        return make_unary(AU_MINUS, value);
    }

    if (token->type == T_INTEGER) {
        EAT(T_INTEGER);
        return make_integer(token->value);
    }

    if (token->type == T_FLOAT) {
        EAT(T_FLOAT);
        return make_float(token->value);
    }

    if (token->type == T_LPAREN) {
        EAT(T_LPAREN);
        Ast *in_expr = expr();
        ASSERT_ERROR(in_expr);
        EAT(T_RPAREN);
        return in_expr;
    }

    return variable();
}

Ast *term() {
    Ast *lvalue = factor();
    ASSERT_ERROR(lvalue);

    Token *op = current_token();
    while(op->type == T_SLASH || op->type == T_STAR) {
        if (op->type == T_SLASH) {
            EAT(T_SLASH);
            Ast *rvalue = factor();
            ASSERT_ERROR(rvalue);
            lvalue = make_binop(BINOP_DIV, lvalue, rvalue);
        }

        if (op->type == T_STAR) {
            EAT(T_STAR);
            Ast *rvalue = factor();
            ASSERT_ERROR(rvalue);
            lvalue = make_binop(BINOP_MUL, lvalue, rvalue);
        }
        op = current_token();
    }

    return lvalue;
}

Ast *expr() 
{
    Ast *lvalue = term();
    ASSERT_ERROR(lvalue);
    Token *op = current_token();
    while (op->type == T_PLUS || op->type == T_MINUS) {
        if (op->type == T_PLUS) {
            EAT(T_PLUS);
            Ast *rvalue = term();
            ASSERT_ERROR(rvalue);
            lvalue = make_binop(BINOP_ADD, lvalue, rvalue);
        }

        if (op->type == T_MINUS) {
            EAT(T_MINUS);
            Ast *rvalue = term();
            ASSERT_ERROR(rvalue);
            lvalue = make_binop(BINOP_SUB, lvalue, rvalue);
        }

        op = current_token();
    }

    return lvalue;
}