#ifndef __RUNTIME_H
#define __RUNTIME_H
#include <stdio.h>

#include "vm.h"
#include "token.h"
#include "ast.h"
#include "parser.h"
#include "interpreter.h"

void runtime(int, char **, RuntimePipe *);

#endif