#include "source.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

SourceCodeFile read_source_code(const char *file_name)
{
    FILE* file = fopen(file_name, "r");
    if (!file) {
        fprintf(stderr, "Can't open source code file `%s`", file_name);
        exit(1);
    }
    
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = malloc(file_size + 1);

    assert(fread(content, 1, file_size, file) == file_size);
    content[file_size] = 0;
    fclose(file);

    SourceCodeFile source_code;
    source_code.content = content;
    source_code.file_name = file_name;
    source_code.size = file_size;
    source_code.line = 1;
    source_code.column = 1;
    return source_code;
}
