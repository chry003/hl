#include "include/hl.h"
#include "include/io.h"

#include "include/lexer.h"
#include "include/parser.h"

#include <stdlib.h>

void hl_compiler(char* src)
{
    lexer_T* lexer = init_lexer(src);
    parser_T* parser = init_parser(lexer);
    AST_T* root = parser_parse(parser);
    stack_print(parser->stack);

    char* begin = "section .text\n"
                  "\tglobal main\n"
                  "main:\n"
                  "\tpush ebp\n"
                  "\tmov ebp, esp\n";
    char* end =   "\tmov eax, [ebp - 4]\n"
                  "\tpop ebp\n"
                  "\tret";

    char* final = calloc((strlen(begin) + strlen(parser->local_asm) + strlen(end) + 128), sizeof(char));
    strcat(final, begin); strcat(final, parser->local_asm); strcat(final, end);

    write_file("main.asm", final);

    /*
    while (tok->type != EOFL)
    {
        printf("%s\n", token_to_str_simple(tok));
        tok = lexer_next_token(lexer);
    }
    */
}

void hl_compile_file(const char* filename)
{
    char* word = read_file(filename);
    hl_compiler(word);
    free(word);
}
