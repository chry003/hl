#include "include/hl.h"
#include "include/io.h"
#include "include/macro.h"

#include "include/lexer.h"
#include "include/parser.h"
#include "include/codegen.h"

#include <stdlib.h>

void hl_compiler(char* src)
{
    lexer_T* lexer = init_lexer(src);
    parser_T* parser = init_parser(lexer);
    AST_T* root = parser_parse(parser);
    codegen_T* codegen = init_codegen(parser->stack);
    codegen_parse_ast(codegen, root);
}

void hl_compile_file(const char* filename)
{
    char* word = read_file(filename);
    hl_compiler(word);
    free(word);
}
