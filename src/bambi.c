#include <assert.h>
#include "vm.h"
#include "tokenizer.h"
#include "parser.h"
#include "context.h"
#include "interpreter.h"
#include "source.h"
#include "ast.h"
#include <string.h>

int main(int argc, char** argv)
{
    bool debug = false;
    char *file_name = NULL;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "debug") == 0) {
            debug = true;
        } else {
            file_name = argv[i];
        }
    }

    assert(argc <= 3 && "Unexpected amount of arguments!");

    SourceCodeFile file = read_source_code(file_name);

    Lexer lexer = (Lexer) {
        .file = &file
    };

    tokenize(&lexer);

    if (debug) {
        debug_tokenize(&lexer);
    }

    Ast *ast_tree = program(&lexer);

    Context global = create_stack(NULL);
    interpret(&global, ast_tree);
    drop_stack(&global);
    cleanup(&lexer);
    return EXIT_SUCCESS;
}
