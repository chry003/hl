#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TOKEN_STRUCT
{
    char* value;
    enum
    {
        // FOR LIT
        ID,
        INT,

        // FOR PUNCTUATIONS OR SYMBOL
        COLON,
        SEMI,
        UNDERSCORE,
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,

        // FOR ARTHMETIC THINGS

        // FOR COMPARISON
        EQUAL,
        LT,
        GT,
        NOT_EQUAL,

        // MIS
        EOFL
    } type;
} token_T;

token_T* init_token(char* value, int type);

const char* token_type_to_str(int type);

char* token_to_str(token_T* token);

char* token_to_str_simple(token_T* token);
