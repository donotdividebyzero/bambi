#ifndef __STRING_H
#define __STRING_H

#include <string.h>
#include <stdlib.h>

char *ltrim(char *str, const char *seps);

char *rtrim(char *str, const char *seps);

char *trim(char *str, const char *seps);

#endif