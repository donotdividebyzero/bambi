#ifndef __VM_H
#define __VM_H
#ifndef NULL
    #define NULL (void *)0
#endif

#include "types.h"

bool string_equal(const String *, const String *);
String string_concat(const String *, const String *);

void cleanup(Lexer *);
void print_compiler_error(Token *, const char *);
void print_compiler_note(Token *, const char *);


bool is_truthy(Value *);
bool is_equal(Value *, Value *);

#endif
