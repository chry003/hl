#pragma once

#include "list.h"

typedef struct AST_STRUCT
{
    enum
    {
        AST_COMPOUND = 0,
        AST_VARIABLE_ASSIGN,
        AST_VARIABLE_DEFINE,
        AST_VARIABLE,
        AST_IF,
        AST_CONDITION,
        AST_INT,
        AST_NOP
    } type;

    enum
    {
        ptr,
        byte,
        i16,
        i32
    } data_type;

    char* name;
    struct AST_STRUCT* value;
    list_T* body;

    int int_value;
    char char_value;
    char* string_value;

} AST_T;

AST_T* init_ast(int type);

void ast_print(AST_T* ast);

char* ast_get_data_type_name(AST_T* ast);
