#include "include/hl.h"
#include "include/io.h"
#include "include/macro.h"

#include "include/lexer.h"
#include "include/parser.h"
#include "include/codegen.h"

#include <stdlib.h>

void hl_compiler(char* src)
{
    // err(0, "moving parser_generated_assembly to codegen, not implemented yet");

    lexer_T* lexer = init_lexer(src);
    parser_T* parser = init_parser(lexer);
    AST_T* root = parser_parse(parser);
    codegen_T* codegen = init_codegen(parser->stack);
    codegen_parse_ast(codegen, root);

    char* begin = "section .text\n"
                  "\tglobal main\n"
                  "main:\n"
                  "\tpush ebp\n"
                  "\tmov ebp, esp\n";
    char* end =   "\tmov eax, [ebp - 9]\n"
                  "\tpop ebp\n"
                  "\tret";
}

void hl_compile_file(const char* filename)
{
    char* word = read_file(filename);
    hl_compiler(word);
    free(word);
}
