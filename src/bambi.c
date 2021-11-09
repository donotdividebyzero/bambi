#include <assert.h>
#include "vm.h"
#include "tokenizer.h"
#include "parser.h"
#include "context.h"
#include "interpreter.h"
#include "source.h"
#include "ast.h"

int main(int argc, char** argv)
{
    assert(argc >= 2 && "Unexpected amount of arguments!");
    SourceCodeFile file = read_source_code(argv[1]);
    Lexer lexer = (Lexer) {
        .file = &file
    };
    tokenize(&lexer);
    Ast *ast_tree = program(&lexer);

    Context global = create_stack(NULL);
    interpret(&global, ast_tree);
    drop_stack(&global);
    cleanup(&lexer);
    return EXIT_SUCCESS;
}
