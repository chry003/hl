#include "include/codegen.h"
#include "include/macro.h"
#include <string.h>

codegen_T* init_codegen(stack_T* stack)
{
    codegen_T* new_codegen = calloc(1, sizeof(struct CODEGEN_STRUCT));
    new_codegen->label_code = calloc(1, sizeof(char));
    new_codegen->stack = stack;

    return new_codegen;
}

void codegen_compound(codegen_T* codegen, AST_T* ast)
{
    for (int i = 0; i < ast->body->size; i++)
    {
        AST_T* ast_child = (AST_T*)ast->body->items[i];
        codegen_parse_ast(codegen, ast_child);
    }
}

void codegen_variable(codegen_T* codegen, AST_T* ast)
{
    int lvalue_location = stack_get_location(codegen->stack, ast->name); /*- (stack_get_index(codegen->stack, ast->name) - (codegen->stack->list->size - 1));*/

    if ( ast->value->type == AST_INT )
    {
        switch(ast->data_type)
        {
            case byte: codegen_append(codegen, "\tmov byte [ebp - %i], %i\n", lvalue_location, ast->value->int_value); break;
            case i16: codegen_append(codegen, "\tmov word [ebp - %i], %i\n", lvalue_location, ast->value->int_value); break;
            case i32: codegen_append(codegen, "\tmov dword [ebp - %i], %i\n", lvalue_location, ast->value->int_value); break;
            case ptr: err(0, "pointers are not implemented yet.");
        }
    }
    else if ( ast->value->type == AST_VARIABLE )
    {
        int lvalue_size = stack_get_location(codegen->stack, ast->name) - stack_get_location_with_index(codegen->stack, stack_get_index(codegen->stack, ast->name) - 1);
        int rvalue_size = stack_get_location(codegen->stack, ast->value->name) - stack_get_location_with_index(codegen->stack, stack_get_index(codegen->stack, ast->value->name) - 1);

        int rvalue_loc = stack_get_location(codegen->stack, ast->value->name);
        int lvalue_loc = stack_get_location(codegen->stack, ast->name);

        char* rname = calloc(1, sizeof(char));
        char* rmov_bit = calloc(1, sizeof(char));

        switch(ast->value->data_type)
        {
            case byte: rname = "byte"; break;
            case i16: rname = "word"; break;
            case i32: rname = "dword"; break;
            case ptr: err(0, "pointers are not implemented yet.");
        }

        char* template = "\t%s %s, %s [ebp - %i]\n"
                         "\tmov %s [ebp - %i], %s\n";

        if (rvalue_size >= lvalue_size)
        {
            rmov_bit = (rvalue_size <= 2 ? "movzx" : "mov");

            switch(ast->data_type)
            {
                case byte: codegen_append(codegen, template, rmov_bit, "eax", rname, rvalue_loc, "byte", lvalue_loc, "al"); break;
                case i16: codegen_append(codegen, template, rmov_bit, "eax", rname, rvalue_loc, "word", lvalue_loc, "ax"); break;
                case i32: codegen_append(codegen, template, rmov_bit, "eax", rname, rvalue_loc, "dword", lvalue_loc, "eax"); break;
                case ptr: err(0, "pointers are not implemented yet.");
            }
        }
        else
        {
            rmov_bit = (rvalue_size <= 2 ? "movsx" : "mov");

            switch(ast->data_type)
            {
                case byte: codegen_append(codegen, template, rmov_bit, "al", rname, rvalue_loc, "byte", lvalue_loc, "al"); break;
                case i16: codegen_append(codegen, template, rmov_bit, "ax", rname, rvalue_loc, "word", lvalue_loc, "ax"); break;
                case i32: codegen_append(codegen, template, rmov_bit, "eax", rname, rvalue_loc, "dword", lvalue_loc, "eax"); break;
                case ptr: err(0, "pointers are not implemented yet.");
            }
        }
    }
}

void codegen_if(codegen_T* codegen, AST_T* ast)
{
    err(0, "<if> statement cannot generate assembly code, implementation not done yet.");
}

void codegen_parse_ast(codegen_T* codegen, AST_T* ast)
{
    switch(ast->type)
    {
        case AST_VARIABLE_ASSIGN: codegen_variable(codegen, ast); break;
        case AST_IF: codegen_if(codegen, ast); break;
        case AST_COMPOUND: codegen_compound(codegen, ast); break;
        default: err(0, "[Codegen]: only variables are compilable, others are not implemented yet.");
    }

    codegen_write(codegen);
}

void codegen_extend_with_source(codegen_T* codegen, char* src)
{
    codegen->label_code = realloc(codegen->label_code, (strlen(src) + WRITE_SIZE) * sizeof(char));
    strcat(codegen->label_code, src);
}

void codegen_write(codegen_T* codegen)
{
    char* begin = "section .text\n"
                  "\tglobal main\n"
                  "main:\n"
                  "\tpush ebp\n"
                  "\tmov ebp, esp\n";
    char* end =   "\tmov eax, [ebp - 4]\n"
                  "\tpop ebp\n"
                  "\tret";

    char* out = calloc(strlen(begin) + strlen(codegen->label_code) + strlen(end) + WRITE_SIZE, sizeof(char));
    strcat(out, begin); strcat(out, codegen->label_code); strcat(out, end);
    write_file("main.asm", out);
    system("nasm -f elf -o ./main.o ./main.asm");
    system("gcc -no-pie -m32 -o ./main ./main.o");
    system("rm ./main.asm ./main.o");
}
