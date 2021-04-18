#ifndef __INTERPRETER_H
#define __INTERPRETER_H

#include "vm.h"
#include "ast.h"
#include "context.h"

Ast *interpret(RuntimePipe *, Context *, Ast *);

#endif