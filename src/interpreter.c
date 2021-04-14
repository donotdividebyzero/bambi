#include "interpreter.h"

Ast *interpret_binop(RuntimePipe *pipe, Ast *binop)
{
    ASSERT_ERROR(binop);
    Ast *left = interpret(pipe, binop->op->lvalue);
    Ast *right = interpret(pipe, binop->op->rvalue);

    ASSERT_ERROR(left);
    ASSERT_ERROR(right);

    if (left->type != AV_NUMERIC || right->type != AV_NUMERIC) {
        return make_error("Can't perform bin operation on no numeric values!");
    }

    Ast_Numeric *result = malloc(sizeof(Ast_Numeric));
    result->type = AN_INTEGER;

    if (left->value->numeric->type == AN_FLOAT || right->value->numeric->type == AN_FLOAT) {
        result->type = AN_FLOAT;
    }

    switch (binop->op->type) {
        case BINOP_ADD: {
            switch(result->type) {
                case AN_FLOAT: {
                    if (left->value->numeric->type == AN_INTEGER && right->value->numeric->type == AN_FLOAT) {
                        result->f = left->value->numeric->l + right->value->numeric->f;
                    } else if (left->value->numeric->type == AN_FLOAT && right->value->numeric->type == AN_INTEGER) {
                        result->f = left->value->numeric->f + (float)right->value->numeric->l;
                    } else if (left->value->numeric->type == AN_FLOAT && right->value->numeric->type == AN_FLOAT) {
                        result->f = left->value->numeric->f + right->value->numeric->f;
                    }
                } break;
                case AN_INTEGER: {
                    result->l = left->value->numeric->l + right->value->numeric->l;
                } break;
                default: break;
            }
            return make_numeric(result);
        } break;
        case BINOP_SUB: {
            switch(result->type) {
                case AN_FLOAT: {
                    if (left->value->numeric->type == AN_INTEGER && right->value->numeric->type == AN_FLOAT) {
                        result->f = left->value->numeric->l - right->value->numeric->f;
                    } else if (left->value->numeric->type == AN_FLOAT && right->value->numeric->type == AN_INTEGER) {
                        result->f = left->value->numeric->f - (float)right->value->numeric->l;
                    } else if (left->value->numeric->type == AN_FLOAT && right->value->numeric->type == AN_FLOAT) {
                        result->f = left->value->numeric->f - right->value->numeric->f;
                    }
                } break;
                case AN_INTEGER: {
                    result->l = left->value->numeric->l - right->value->numeric->l;
                } break;
                default: break;
            }
            return make_numeric(result);
        } break;
        case BINOP_MUL: {
            switch(result->type) {
                case AN_FLOAT: {
                    if (left->value->numeric->type == AN_INTEGER && right->value->numeric->type == AN_FLOAT) {
                        result->f = left->value->numeric->l * right->value->numeric->f;
                    } else if (left->value->numeric->type == AN_FLOAT && right->value->numeric->type == AN_INTEGER) {
                        result->f = left->value->numeric->f * (float)right->value->numeric->l;
                    } else if (left->value->numeric->type == AN_FLOAT && right->value->numeric->type == AN_FLOAT) {
                        result->f = left->value->numeric->f * right->value->numeric->f;
                    }
                } break;
                case AN_INTEGER: {
                    result->l = left->value->numeric->l * right->value->numeric->l;
                } break;
                default: break;
            }
            return make_numeric(result);
        } break;
        case BINOP_DIV: {
            if (right->value->numeric->type == AN_INTEGER && right->value->numeric->l == 0) {
                return make_error("You can't do division by 0!");
            }

            if (right->value->numeric->type == AN_FLOAT && right->value->numeric->f == 0) {
                return make_error("You can't do division by 0!");
            }

            if (left->value->numeric->type == AN_INTEGER && right->value->numeric->type == AN_FLOAT) {
                result->f = left->value->numeric->l / right->value->numeric->f;
            } else if (left->value->numeric->type == AN_FLOAT && right->value->numeric->type == AN_INTEGER) {
                result->f = left->value->numeric->f / right->value->numeric->l;
            } else if (left->value->numeric->type == AN_FLOAT && right->value->numeric->type == AN_FLOAT) {
                result->f = left->value->numeric->f / right->value->numeric->f;
            } else {
                result->f = left->value->numeric->l / right->value->numeric->l;
            }

            return make_numeric(result);
        } break;
        case BINOP_MOD: {
            switch(result->type) {
                case AN_INTEGER: {
                    result->l = left->value->numeric->l % right->value->numeric->l;
                } break;
                case (AN_FLOAT): {
                    return make_error("Module operation is allowed only on integer values!");
                }
                default: break;
            }
            return make_numeric(result);
        } break;
    }

    return make_error("Unexpected binary operation!");
}

/**
 * Entry point to interpreter
 */
Ast *interpret(RuntimePipe *pipe, Ast *root)
{
    ASSERT_ERROR(root);
    if (root->type == AT_BINOP) return interpret_binop(pipe, root);
    return root;
}