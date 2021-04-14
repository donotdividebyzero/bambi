#ifndef __INTERPRETER_H
#define __INTERPRETER_H

#include "vm.h"
#include "ast.h"

Ast *interpret(RuntimePipe *, Ast *);

#endif