#include "ast.h"
#include <string.h>

typedef struct Context_Variable
{
    Ast *var;
    Ast *value;
} Context_Variable;

typedef struct Context
{
    struct Context *parent;
    char *ctx_name;
    size_t size;
    Context_Variable **vars;
} Context;

void create_global_context();

void free_global_context();

Context *get_global_context();

Context *make_context(Context *, char *);

Context_Variable *find_in_context(Context *, char *);

void push_to_context(Context *, Context_Variable *);
