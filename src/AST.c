#include "include/AST.h"
#include "include/macro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

AST_T* init_ast(int type)
{
    AST_T* ast = calloc(1, sizeof(struct AST_STRUCT));
    ast->type = type;

    if (type == AST_COMPOUND)
        ast->body = init_list(sizeof(struct AST_STRUCT*));

    return ast;
}

void ast_print(AST_T* ast)
{
    char* template = "\n[Name]: %s\n"
                     "[int_value]: %i\n"
                     "[char_value]: %s\n"
                     "[string_value]: %s\n"
                     "[data_type]: %s\n"
                     "[type]: %s\n"
                     "[size_of_body]: %s\n"
                     "[value_type]: %s\n";
    char* src = calloc(strlen(template) + 256 * sizeof(char), sizeof(char));

    char* size_of_body = calloc(1, sizeof(char));
    char* ast_type = calloc(1, sizeof(char));
    char* ast_value_type = calloc(1, sizeof(char));
    char* data_type = calloc(1, sizeof(char));

    if (ast->body) sprintf(size_of_body, "%i", (int)ast->body->size);
    else size_of_body = "(null)";

    switch (ast->type)
    {
        case AST_COMPOUND: ast_type = "AST_COMPOUND"; break;
        case AST_VARIABLE_ASSIGN: ast_type = "AST_VARIABLE_ASSIGN"; break;
        case AST_VARIABLE_DEFINE: ast_type = "AST_VARIABLE_DEFINE"; break;
        case AST_VARIABLE: ast_type = "AST_VARIABLE"; break;
        case AST_IF: ast_type = "AST_IF"; break;
        case AST_CONDITION: ast_type = "AST_CONDITION"; break;
        case AST_INT: ast_type = "AST_INT"; break;
        case AST_NOP: ast_type = "AST_NOP"; break;
    }

    if (ast->value)
    {
        switch (ast->value->type)
        {
            case AST_COMPOUND: ast_value_type = "AST_COMPOUND"; break;
            case AST_VARIABLE_ASSIGN: ast_value_type = "AST_VARIABLE_ASSIGN"; break;
            case AST_VARIABLE_DEFINE: ast_value_type = "AST_VARIABLE_DEFINE"; break;
            case AST_VARIABLE: ast_value_type = "AST_VARIABLE"; break;
            case AST_IF: ast_value_type = "AST_IF"; break;
            case AST_CONDITION: ast_value_type = "AST_CONDITION"; break;
            case AST_INT: ast_value_type = "AST_INT"; break;
            case AST_NOP: ast_value_type = "AST_NOP"; break;
        }
    }
    else ast_value_type = "(null)";

    switch (ast->data_type)
    {
        case ptr: data_type = "ptr"; break;
        case byte: data_type = "byte"; break;
        case i16: data_type = "i16"; break;
        case i32: data_type = "i32"; break;
    }

    sprintf(
            src,
            template,
            ast->name,
            ast->int_value,
            ast->char_value,
            ast->string_value,
            data_type,
            ast_type,
            size_of_body,
            ast_value_type
           );
    printf("%s\n", src);
}

char* ast_get_data_type_name(AST_T* ast)
{
    char* value = calloc(1, sizeof(char));

    switch(ast->data_type)
    {
        case byte: value = "byte"; break;
        case i16: value = "word"; break;
        case i32: value = "dword"; break;
        case ptr: err(0, "pointers are not implemented yet.");
    }

    return value;
}
