#ifndef __TESTSUITE_H
#define __TESTSUITE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include "vm.h"
#include "string.h"

void testsuite(int argc, char **argv, RuntimePipe *);

#endif

