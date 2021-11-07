#include "context.h"

ContextVariable *context_lookup(Context *ctx, const String *id)
{
    ContextVariable *variable = ctx->vars;
    while (variable != NULL)
    {
        Variable *var = &variable->var->var;
        if (string_equal(&var->name, id)) return variable;
        variable = variable->next;
    }

    return NULL;
}

ContextVariable *find_in_context(Context *ctx, const String *id)
{
    if (id == NULL || ctx == NULL) {
        return NULL;
    }
    
    ContextVariable *variable = context_lookup(ctx, id);
    if (variable) return variable;
    
    if (ctx->parent != NULL)
    {
        variable = context_lookup(ctx->parent, id);
        if (variable) return variable;
    }

    return NULL;
}

void push_to_context(Context *ctx, ContextVariable *var)
{
    if (var != NULL) {
        var->next = NULL;
        ContextVariable *stack = ctx->vars;
        if (stack) {
            while (stack->next != NULL) stack = stack->next;
            stack->next = var;
        } else {
            ctx->vars = var;
        }
    }
}
