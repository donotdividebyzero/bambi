#include "interpreter.h"
#include "token.h"

Ast factor()
{
    Token *token = current_token();
    Tokenizer_Error is_eaten;
    if (token->type == LEFT_PAREN) {
        eat_token(LEFT_PAREN);
        Ast in_expr = expr();
        if (in_expr.error != NULL) {
            return in_expr;
        }

        is_eaten = eat_token(RIGHT_PAREN);
        if (is_eaten.error != NULL) {
            return (Ast) {
                .error = is_eaten.error
            };
        }

        return in_expr;
    }

    is_eaten = eat_token(INTEGER);
    if (is_eaten.error != NULL) {
        return (Ast) {
            .error = is_eaten.error
        };
    }

    return (Ast) {
        .error = NULL,
        .number.l = strtol(token->value, NULL, 10)
    };
}

Ast term() {
    Ast factor_expr = factor();
    if (factor_expr.error != NULL) {
        return factor_expr;
    }

    long result = factor_expr.number.l;
    Token *op = current_token();
    Tokenizer_Error is_eaten;
    while(op->type == SLASH || op->type == STAR) {
        if (op->type == SLASH) {
            is_eaten = eat_token(SLASH);
            if (is_eaten.error != NULL) {
                return (Ast) {
                    .error = is_eaten.error
                };
            }

            Ast factor_expr = factor();
            if (factor_expr.error != NULL) {
                return factor_expr;
            }

            if (factor_expr.number.l == 0) {
                return (Ast) {
                    .error = "You can't divide by 0!"
                };
            }

            result = result / factor_expr.number.l;
        }

        if (op->type == STAR) {
            is_eaten = eat_token(STAR);
            if (is_eaten.error != NULL) {
                return (Ast) {
                    .error = is_eaten.error
                };
            }

            Ast factor_expr = factor();
            if (factor_expr.error != NULL) {
                return factor_expr;
            }

            result = result * factor_expr.number.l;
        }
        op = current_token();
    }

    return (Ast) {
        .error = NULL,
        .number.l = result
    };
}

Ast expr() 
{
    Ast term_expr = term();
    if (term_expr.error != NULL) {
        return term_expr;
    }

    Token *op = current_token();
    Tokenizer_Error is_eaten;
    long result = term_expr.number.l;

    while (op->type == PLUS || op->type == MINUS) {
        if (op->type == PLUS) {
            is_eaten = eat_token(PLUS);
            if (is_eaten.error != NULL) {
                return (Ast) {
                    .error = is_eaten.error
                };
            }

            Ast term_expr = term();
            if (term_expr.error != NULL) {
                return term_expr;
            }

            result = result + term_expr.number.l;
        }

        if (op->type == MINUS) {
            is_eaten = eat_token(MINUS);
            if (is_eaten.error != NULL) {
                return (Ast) {
                    .error = is_eaten.error
                };
            }

            Ast term_expr = term();
            if (term_expr.error != NULL) {
                return term_expr;
            }

            result = result - term_expr.number.l;
        }
        op = current_token();
    }

    return (Ast) {
        .error = NULL,
        .number.l = result
    };
}



