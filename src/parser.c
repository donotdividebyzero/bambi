#include "parser.h"

Ast *factor()
{
    Token *token = current_token();
    if (token->type == INTEGER) {
        EAT(INTEGER);
        return make_integer(token->value);
    }

    if (token->type == FLOAT) {
        EAT(FLOAT);
        return make_float(token->value);
    }

    if (token->type == LEFT_PAREN) {
        eat_token(LEFT_PAREN);
        Ast *in_expr = expr();
        ASSERT_ERROR(in_expr);
        EAT(RIGHT_PAREN);
        return in_expr;
    }

    char *error_buffer = malloc(100);
    sprintf(error_buffer, "Unexpected token, exptected NUMERIC | LEFT_PAREN, got : '%s'", token_type_to_str(token->type));
    return make_error(error_buffer);
}

Ast *term() {
    Ast *lvalue = factor();
    if (lvalue->error != NULL) {
        return lvalue;
    }
    Token *op = current_token();
    while(op->type == SLASH || op->type == STAR) {
        if (op->type == SLASH) {
            EAT(SLASH);
            Ast *rvalue = factor();
            ASSERT_ERROR(rvalue);
            return make_binop(BINOP_DIV, lvalue, rvalue);
        }

        if (op->type == STAR) {
            EAT(STAR);
            Ast *rvalue = factor();
            ASSERT_ERROR(rvalue);
            return make_binop(BINOP_MUL, lvalue, rvalue);
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
    while (op->type == PLUS || op->type == MINUS) {
        if (op->type == PLUS) {
            EAT(PLUS);
            Ast *rvalue = term();
            ASSERT_ERROR(rvalue);
            return make_binop(BINOP_ADD, lvalue, rvalue);
        }

        if (op->type == MINUS) {
            EAT(MINUS);
            Ast *rvalue = term();
            ASSERT_ERROR(rvalue);
            return make_binop(BINOP_SUB, lvalue, rvalue);
        }

        op = current_token();
    }

    return lvalue;
}