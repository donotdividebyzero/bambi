#include "interpreter.h"

Ast *interpret_unary(RuntimePipe *pipe, Context *ctx, Ast *unary)
{
    Ast *value = interpret(pipe, ctx, unary->unary->expr);
    ASSERT_ERROR(value);
    if (value->type == AT_VALUE && value->value->type == AV_NUMERIC) {
        if (unary->unary->type == AU_MINUS) {
            if (value->value->numeric->type == AN_FLOAT) value->value->numeric->f = -1.0 * value->value->numeric->f;
            else value->value->numeric->l = -1 * value->value->numeric->l;
        }
    }

    return value;
}

Ast *interpret_binop(RuntimePipe *pipe, Context *ctx, Ast *binop)
{
    ASSERT_ERROR(binop);
    Ast *left = interpret(pipe, ctx, binop->op->lvalue);
    Ast *right = interpret(pipe, ctx, binop->op->rvalue);

    ASSERT_ERROR(left);
    ASSERT_ERROR(right);

    if ((left->type != AT_VALUE || right->type != AT_VALUE) || (left->value->type != AV_NUMERIC || right->value->type != AV_NUMERIC)) {
        return make_error("Can't perform bin operation on no numeric values!");
    }

    Ast_Numeric *result = malloc(sizeof(Ast_Numeric));
    result->type = AN_FLOAT;

    if (left->value->numeric->type == AN_INTEGER && right->value->numeric->type == AN_INTEGER) {
        result->type = AN_INTEGER;
    }

    switch (binop->op->type) {
        case BINOP_ADD: {
            switch(result->type) {
                case AN_FLOAT: {
                    if (left->value->numeric->type == AN_INTEGER && right->value->numeric->type == AN_FLOAT) {
                        result->f = (float)left->value->numeric->l + right->value->numeric->f;
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
        } break;
        case BINOP_SUB: {
            switch(result->type) {
                case AN_FLOAT: {
                    if (left->value->numeric->type == AN_INTEGER && right->value->numeric->type == AN_FLOAT) {
                        result->f = (float)left->value->numeric->l - right->value->numeric->f;
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
        } break;
        case BINOP_MUL: {
            switch(result->type) {
                case AN_FLOAT: {
                    if (left->value->numeric->type == AN_INTEGER && right->value->numeric->type == AN_FLOAT) {
                        result->f = (float)left->value->numeric->l * right->value->numeric->f;
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
        } break;
        case BINOP_DIV: {
            if (right->value->numeric->type == AN_INTEGER && right->value->numeric->l == 0) {
                return make_error("You can't do division by 0!");
            }

            if (right->value->numeric->type == AN_FLOAT && right->value->numeric->f == 0) {
                return make_error("You can't do division by 0!");
            }

            result->type = AN_FLOAT;

            if (left->value->numeric->type == AN_INTEGER && right->value->numeric->type == AN_FLOAT) {
                result->f = left->value->numeric->l / right->value->numeric->f;
            } else if (left->value->numeric->type == AN_FLOAT && right->value->numeric->type == AN_INTEGER) {
                result->f = left->value->numeric->f / right->value->numeric->l;
            } else if (left->value->numeric->type == AN_FLOAT && right->value->numeric->type == AN_FLOAT) {
                result->f = left->value->numeric->f / right->value->numeric->f;
            } else {
                result->f = left->value->numeric->l / right->value->numeric->l;
            }
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
        } break;
    }

    return make_numeric(result);
}

Ast *interpret_assigment(RuntimePipe *pipe, Context *ctx, Ast *node)
{
    printf("Try tp allocate\n");
    ASSERT_ERROR(node);
    Context_Variable *ctx_var = find_in_context(ctx, node->assigment->var->var->name);
    if (ctx_var) {
        if (ctx_var->var->var->is_const) {
            return make_error("You can't reassign const value!");
        }
        ctx_var->value = interpret(pipe, ctx, node->assigment->expr);
        push_to_context(ctx, ctx_var);
        return NULL;
    }
    ctx_var = malloc(sizeof(Context_Variable));
    ctx_var->var = node->assigment->var;
    ctx_var->value = interpret(pipe, ctx, node->assigment->expr);
    push_to_context(ctx, ctx_var);

    return NULL;
}

Ast *interpret_variable(RuntimePipe *pipe, Context *ctx, Ast *node)
{
    (void)pipe;
    ASSERT_ERROR(node);
    Context_Variable *var = find_in_context(ctx, node->var->name);
    if (var) {
        return var->value;
    }

    char *err = malloc(100);
    return make_error(err);
}

Ast *interpret_statement_list(RuntimePipe *pipe, Context *ctx, Ast *node)
{
    ASSERT_ERROR(node);
    Ast *stmt = *node->stmt_list->statements;
    for(size_t i = 0; i <= node->stmt_list->size - 1; i++) {
        interpret(pipe, ctx, stmt);
        stmt++;
    }

    return NULL;
}

Ast * interpret(RuntimePipe *pipe, Context *ctx, Ast *root)
{
    ASSERT_ERROR(root);
    printf("Interpret(%d)", root->type);
    if (root->type == AT_VALUE) return root;
    if (root->type == AT_ASSIGMENT) return interpret_assigment(pipe, ctx, root);
    if (root->type == AT_VARIABLE) return interpret_variable(pipe, ctx, root);
    if (root->type == AT_BINOP) return interpret_binop(pipe, ctx, root);
    if (root->type == AT_UNARY) return interpret_unary(pipe, ctx, root);
    if (root->type == AT_EMPTY) return NULL; //@TODO: this might cause some problems in future watch out at this.
    if (root->type == AT_STATEMENT_LIST) return interpret_statement_list(pipe, ctx, root);

    return make_error("Unknown type!");
}