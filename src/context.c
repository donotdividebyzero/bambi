#include "context.h"

Context *global;


Context *make_context(Context *parent, char *ctx_name)
{
    Context *context = malloc(sizeof(Context));
    context->parent = parent;
    context->ctx_name = malloc(strlen(ctx_name));
    strncpy(context->ctx_name, ctx_name, strlen(ctx_name));
    context->size = 0;

    return context;
}

void free_global_context()
{

}
void create_global_context()
{
    global = make_context(NULL, "global");
}

Context *get_global_context()
{
    return global;
}

Context_Variable *find_in_context(Context *ctx, char *id)
{
    for(size_t i = 0; i <= ctx->size-1; i++) if (strcmp(ctx->vars[i]->var->var->name, id) == 0) return ctx->vars[i];
    return NULL;
}

void push_to_context(Context *ctx, Context_Variable *var)
{
    if (var) {
        if (ctx->size == 0) {
            ctx->vars = (Context_Variable **)malloc(sizeof(Context_Variable) * 5);
            ctx->vars[0] = var;
            ctx->size = 1;
        } else {
            if (ctx->size % 5 == 1) {
                ctx->vars = realloc(ctx->vars, sizeof(Context_Variable) * (ctx->size + 5));
            }
            ctx->vars[ctx->size] = var;
            ctx->size = ctx->size + 1;
        }
    }
}