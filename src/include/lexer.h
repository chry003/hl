#pragma once

#include "tokens.h"
#include <stdio.h>

typedef struct LEXER_STRUCT
{
    char* src; // source file
    size_t src_size; // source size
    char c; // char
    unsigned int i; // index
} lexer_T;


lexer_T* init_lexer(char* src);

void lexer_advance(lexer_T* lexer);

token_T* lexer_next_token(lexer_T* lexer);
