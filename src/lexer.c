#include "include/lexer.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

lexer_T* init_lexer(char* src)
{
    lexer_T* lexer = calloc(1, sizeof(struct LEXER_STRUCT));
    lexer->src = src;
    lexer->src_size = strlen(src);
    lexer->i = 0;
    lexer->c = src[lexer->i];

    return lexer;
}

void lexer_advance(lexer_T* lexer)
{
    if (lexer->i < lexer->src_size && lexer->c != '\0')
    {
        lexer->i += 1;
        lexer->c = lexer->src[lexer->i];
    }
}

token_T* lexer_advance_current(lexer_T* lexer, int type)
{
    char* value = calloc(1, sizeof(char));
    value[0] = lexer->c;
    value[1] = '\0';

    token_T* token = init_token(value, type);
    lexer_advance(lexer);

    return token;
}

void lexer_skip_whitespace(lexer_T* lexer)
{
    while
        (
            lexer->c == 13 ||
            lexer->c == 10 ||
            lexer->c == ' ' ||
            lexer->c == '\t'
        ) lexer_advance(lexer);
}

token_T* lexer_parse_id(lexer_T* lexer)
{
    char* word = calloc(1, sizeof(char));
    while(isalpha(lexer->c))
    {
        word = realloc(word, (strlen(word) + 128) * sizeof(char));
        strcat(word, &lexer->c);
        lexer_advance(lexer);
    }
    return init_token(word, ID);
}

token_T* lexer_parse_int(lexer_T* lexer)
{
    char* word = calloc(1, sizeof(char));
    while(isdigit(lexer->c))
    {
        word = realloc(word, (strlen(word) + 128) * sizeof(char));
        strcat(word, &lexer->c);
        lexer_advance(lexer);
    }
    return init_token(word, INT);
}

token_T* lexer_next_token(lexer_T* lexer)
{
    while (lexer->c != '\0')
    {
        lexer_skip_whitespace(lexer);

        if (isalpha(lexer->c)) return lexer_parse_id(lexer);
        if (isdigit(lexer->c)) return lexer_parse_int(lexer);

        switch (lexer->c)
        {
            case ':': return lexer_advance_current(lexer, COLON);
            case ';': return lexer_advance_current(lexer, SEMI);
            case '_': return lexer_advance_current(lexer, UNDERSCORE);
            case '(': return lexer_advance_current(lexer, LPAREN);
            case ')': return lexer_advance_current(lexer, RPAREN);
            case '{': return lexer_advance_current(lexer, LBRACE);
            case '}': return lexer_advance_current(lexer, RBRACE);
            case '=': return lexer_advance_current(lexer, EQUAL);
            case '>': return lexer_advance_current(lexer, LT);
            case '<': return lexer_advance_current(lexer, GT);
            case '!': return lexer_advance_current(lexer, NOT_EQUAL);
            case '\0': break;
            default: printf("[Lexer]: Unexpected char `%c`\n", lexer->c); exit(1); lexer_advance(lexer);
        }
    }

    return init_token(NULL, EOFL);
}
