#pragma once

#include "macro.h"
#include "AST.h"
#include "io.h"
#include "stack_frame.h"

#define WRITE_SIZE 104857600

typedef struct CODEGEN_STRUCT
{
    char* label_code;
    stack_T* stack;
} codegen_T;

codegen_T* init_codegen(stack_T* stack);

void codegen_parse_ast(codegen_T* codegen, AST_T* ast);

#define codegen_append(codegen, temp, ...) {\
        char* src = calloc(strlen(temp) + 8, sizeof(char)); \
        sprintf(src, temp, __VA_ARGS__); \
        codegen_extend_with_source(codegen, src);\
                                            }

void codegen_extend_with_source(codegen_T* codegen, char* src);

void codegen_write(codegen_T* codegen);
