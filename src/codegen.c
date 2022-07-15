#include "include/codegen.h"
#include "include/macro.h"
#include <string.h>

codegen_T* init_codegen(stack_T* stack)
{
    codegen_T* new_codegen = calloc(1, sizeof(struct CODEGEN_STRUCT));
    new_codegen->label_code = calloc(1, sizeof(char));
    new_codegen->stack = stack;
    new_codegen->count_if_statements = 0;

    return new_codegen;
}

char* codegen_get_statement_count(codegen_T* codegen)
{
    char* template = "if_statement_%i";
    char* src = calloc(strlen( template ) + 8, sizeof(char));
    sprintf(src, template, codegen->count_if_statements);

    return src;
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

        rname = ast_get_data_type_name(ast->value);

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
    else if ( ast->value->type == AST_COMPOUND )
    {
        AST_T* first_element = (AST_T*)list_pop_first(ast->value->body);
        AST_T* second_element = (AST_T*)list_pop_first(ast->value->body);
        AST_T* third_element = (AST_T*)list_pop_first(ast->value->body);

        char* first_element_data_type = ast_get_data_type_name(first_element);
        char* third_element_data_type = ast_get_data_type_name(third_element);

        if ( !first_element->name && !third_element->name )
        {
            codegen_append(codegen, "\tmov eax, %i\n", first_element->int_value);
            codegen_append(codegen, "\tcmp eax, %i\n", third_element->int_value);
        }
        else if ( !first_element->name && third_element->name || first_element->name && !third_element->name )
        {
            if (!first_element->name) codegen_append(codegen, "\tcmp %s [ebp - %i], %i\n", third_element_data_type, third_element->int_value, first_element->int_value);
            if (first_element->name) codegen_append(codegen, "\tcmp %s [ebp - %i], %i\n", first_element_data_type, first_element->int_value, third_element->int_value);
        }
        else if ( first_element->name && third_element->name )
        {
            char* move_type_first = calloc(1, sizeof(char));
            char* move_type_third = calloc(1, sizeof(char));
            switch(first_element->data_type)
            {
                case byte: move_type_first = "al"; break;
                case i16: move_type_first = "ax"; break;
                case i32: move_type_first = "eax"; break;
                case ptr: err(0, "pointers are not implemented yet.");
            }
            switch(third_element->data_type)
            {
                case byte: move_type_third = "al"; break;
                case i16: move_type_third = "ax"; break;
                case i32: move_type_third = "eax"; break;
                case ptr: err(0, "pointers are not implemented yet.");
            }

            codegen_append(codegen, "\tmov %s, %s [ebp - %i]\n", move_type_first, first_element_data_type, first_element->int_value);
            codegen_append(codegen, "\tcmp %s, %s [ebp - %i]\n", move_type_third, third_element_data_type, third_element->int_value);
        }

        char* set_condition = calloc(1, sizeof(char));
        switch (second_element->int_value)
        {
            case 0: set_condition = "setne"; break;
            case 1: set_condition = "sete"; break;
            case 2: set_condition = "setg"; break;
            case 3: set_condition = "setl"; break;
            case 4: set_condition = "setle"; break;
            case 5: set_condition = "setge"; break;
        }

        char* move_type = calloc(1, sizeof(char));
        switch(ast->data_type)
        {
            case byte: move_type = "al"; break;
            case i16: move_type = "ax"; break;
            case i32: move_type = "eax"; break;
            case ptr: err(0, "pointers are not implemented yet.");
        }

        codegen_append(codegen, "\t%s al\n", set_condition);
        codegen_append(codegen, "%s", "\tmovzx eax, al\n");
        codegen_append(codegen, "\tmov %s [ebp - %i], %s\n", ast_get_data_type_name(ast), stack_get_location(codegen->stack, ast->name), move_type);
    }
}

void codegen_if(codegen_T* codegen, AST_T* ast)
{
    char* if_statement = codegen_get_statement_count(codegen);
    codegen->count_if_statements++;

    AST_T* first_element = (AST_T*)list_pop_first(ast->value->body);
    AST_T* second_element = (AST_T*)list_pop_first(ast->value->body);
    AST_T* third_element = (AST_T*)list_pop_first(ast->value->body);

    char* first_element_data_type = ast_get_data_type_name(first_element);
    char* third_element_data_type = ast_get_data_type_name(third_element);

    if ( !first_element->name && !third_element->name )
    {
        codegen_append(codegen, "\tmov eax, %i\n", first_element->int_value);
        codegen_append(codegen, "\tcmp eax, %i\n", third_element->int_value);
    }
    else if ( !first_element->name && third_element->name || first_element->name && !third_element->name )
    {
        if (!first_element->name) codegen_append(codegen, "\tcmp %s [ebp - %i], %i\n", third_element_data_type, third_element->int_value, first_element->int_value);
        if (first_element->name) codegen_append(codegen, "\tcmp %s [ebp - %i], %i\n", first_element_data_type, first_element->int_value, third_element->int_value);
    }
    else if ( first_element->name && third_element->name )
    {
        char* move_type_first = calloc(1, sizeof(char));
        char* move_type_third = calloc(1, sizeof(char));
        switch(first_element->data_type)
        {
            case byte: move_type_first = "al"; break;
            case i16: move_type_first = "ax"; break;
            case i32: move_type_first = "eax"; break;
            case ptr: err(0, "pointers are not implemented yet.");
        }
        switch(third_element->data_type)
        {
            case byte: move_type_third = "al"; break;
            case i16: move_type_third = "ax"; break;
            case i32: move_type_third = "eax"; break;
            case ptr: err(0, "pointers are not implemented yet.");
        }

        codegen_append(codegen, "\tmov %s, %s [ebp - %i]\n", move_type_first, first_element_data_type, first_element->int_value);
        codegen_append(codegen, "\tcmp %s, %s [ebp - %i]\n", move_type_third, third_element_data_type, third_element->int_value);
    }

    char* set_condition = calloc(1, sizeof(char));
    switch (second_element->int_value)
    {
        case 0: set_condition = "je"; break;
        case 1: set_condition = "jne"; break;
        case 2: set_condition = "jle"; break;
        case 3: set_condition = "jge"; break;
        case 4: set_condition = "jl"; break;
        case 5: set_condition = "jg"; break;
    }

    codegen_append(codegen, "\t%s %s\n", set_condition, if_statement);

    for ( int i = 0; i < ast->body->size; i++ )
    {
        AST_T* ast_child = (AST_T*)ast->body->items[i];

        if (ast_child->type == AST_IF && i == ast->body->size - 1)
        {
            codegen->count_if_statements--;
            ast_child->int_value = -1;
        }

        codegen_parse_ast(codegen, ast_child);
    }

    if ( ast->int_value != -1 )
        codegen_append(codegen, "%s:\n", if_statement);
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
}

void codegen_extend_with_source(codegen_T* codegen, char* src)
{
    codegen->label_code = realloc(codegen->label_code, (strlen(src) + strlen(codegen->label_code) + 8) * sizeof(char));
    strcat(codegen->label_code, src);
}

void codegen_write(codegen_T* codegen)
{
    char* begin = "section .text\n"
                  "\tglobal main\n"
                  "main:\n"
                  "\tpush ebp\n"
                  "\tmov ebp, esp\n";
    char* end =   "\tmov eax, [ebp - 12]\n"
                  "\tpop ebp\n"
                  "\tret";

    char* out = calloc(strlen(begin) + strlen(codegen->label_code) + strlen(end) + WRITE_SIZE, sizeof(char));
    strcat(out, begin); strcat(out, codegen->label_code); strcat(out, end);

    system("echo -e '[INFO]: Writing to output file'");
    write_file("main.asm", out);

    system("echo '[NASM]: Assembling output file'");
    system("nasm -f elf -o ./main.o ./main.asm");

    system("echo '[INFO]: Linking with object file'");
    system("gcc -no-pie -m32 -o ./main ./main.o");

    system("echo '[INFO]: Removing object file'");
    system("rm ./main.o");
}
