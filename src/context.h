#ifndef __BAMBI_CONTEXT
#define __BAMBI_CONTEXT
#include "vm.h"

ContextVariable *find_in_context(Context *, const String *);
void push_to_context(Context *, ContextVariable *);

Context create_stack(Context *);
void drop_stack(Context *);

#endif
