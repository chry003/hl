#include "include/tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

token_T* init_token(char* value, int type)
{
    token_T* token = calloc(1, sizeof(struct TOKEN_STRUCT));
    token->value = value;
    token->type = type;

    return token;
}

const char* token_type_to_str(int type)
{
    switch (type)
    {
        case ID: return "ID";
        case INT: return "INT";
        case COLON: return "COLON";
        case SEMI: return "SEMI";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case LBRACE: return "LPAREN";
        case RBRACE: return "RPAREN";
        case UNDERSCORE: return "UNDERSCORE";
        case EQUAL: return "EQUAL";
        case LT: return "LT";
        case GT: return "GT";
        case NOT_EQUAL: return "NOT_EQUAL";
        case EOFL: return "EOFL";
    };

    return "token_undefined";
}

char* token_to_str(token_T* token)
{
    const char* type_str = token_type_to_str(token->type);
    const char* template = "<type=%s, int_type=%d, value=%s>";

    char* str = calloc(strlen(type_str) + strlen(template) + 8, sizeof(char));
    sprintf(str, template, type_str, token->type, token->value);

    return str;
}

char* token_to_str_simple(token_T* token)
{
    const char* type_str = token_type_to_str(token->type);
    const char* template = "[%s] %s";

    char* str = calloc(strlen(type_str) + strlen(template) + 8, sizeof(char));
    sprintf(str, template, type_str, token->value);

    return str;
}
