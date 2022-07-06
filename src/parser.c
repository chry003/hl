#include "include/parser.h"
#include "include/macro.h"
#include "include/hl.h"

parser_T* init_parser(lexer_T* lexer)
{
    parser_T* parser = calloc(1, sizeof(struct PARSER_STRUCT));
    parser->lexer = lexer;
    parser->token = lexer_next_token(lexer);
    parser->stack = init_stack();
    parser->local_asm = calloc(1, sizeof(char));

    return parser;
}

int tok_value_match(parser_T* parser, const char* value)
{
    if (strcmp(parser->token->value, value)) return 0;
    return 1;
}

char* tok_expected(parser_T* parser, int type, const char* info)
{
    if (parser->token->type != type)
    {
        const char* template = "[Parser::ERR]: Unexpected token `%s`, expected `%s`\n[Info]: %s";
        char* str = calloc(strlen(template) + 8, sizeof(char));
        sprintf(str, template, token_to_str(parser->token), token_type_to_str(type), info);
        printf("%s\n", str);

        exit(1);
    }
    char* value = calloc(strlen(parser->token->value), sizeof(char));
    strcpy(value, parser->token->value);
    parser_eat(parser, type);

    return value;
}

token_T* parser_eat(parser_T* parser, int type)
{
    if (parser->token->type != type)
    {
        printf("[Parser]: Unexpected tok: %s, expected: %s\n", token_to_str(parser->token), token_type_to_str(type));
        exit(1);
    }

    printf("[Parser]: %s\n", token_to_str(parser->token));
    parser->token = lexer_next_token(parser->lexer);
    return parser->token;
}

AST_T* parser_parse(parser_T* parser)
{
    return parser_parse_compound(parser);
}

AST_T* parser_parse_expr(parser_T* parser)
{
    if (parser->token->type == INT)
    {
        AST_T* ast = init_ast(INT);
        ast->int_value = atoi(parser->token->value);
        parser_eat(parser, INT);

        return ast;
    }
    else err(0, "Expr only supports TOK_TYPE: INT");
}

AST_T* parser_parse_statement(parser_T* parser)
{
    AST_T* ast = init_ast(AST_NOP);
    if (tok_value_match(parser, "var"))
    {
        ast->type = AST_VARIABLE_ASSIGN;
        parser_eat(parser, ID);

        ast->name = calloc(1, sizeof(char));

        char* name = NULL;
        while (parser->token->type == ID || parser->token->type == INT || parser->token->type == UNDERSCORE)
        {
            if (!name) name = calloc(1, sizeof(char));
            name = realloc(name, (strlen(name) + 128) * sizeof(char));
            strcat(name, parser->token->value);
            parser_eat(parser, parser->token->type);
        }

        if (!name) err(0, "<var> statement needs a <name>");

        strcpy(ast->name, name);

        tok_expected(parser, COLON, "<var> <name>: <- missing colon");

        char* type_lit = calloc(1, sizeof(char));
        strcpy(type_lit, tok_expected(parser, ID, "Supported type: ptr(pointer or string), byte(char or one byte), i16(16 bit long signed int), i32(32 bit long signed int)."));

        if (parser->token->type == INT)
        {
            type_lit = realloc(type_lit, (strlen(type_lit) + 8) * sizeof(char));
            strcat(type_lit, tok_expected(parser, INT, "Supported Type Size: i16 or i32"));
        }

        if(!strcmp(type_lit, "ptr")) ast->data_type = ptr;
        else if(!strcmp(type_lit, "byte")) ast->data_type = byte;
        else if(!strcmp(type_lit, "i16")) ast->data_type = i16;
        else if(!strcmp(type_lit, "i32")) ast->data_type = i32;
        else err(0, "Supported type: ptr(pointer or string), byte(char or one byte), i16(16 bit long signed int), i32(32 bit long signed int).");

        tok_expected(parser, EQUAL, "<var> <name>: <type> = <- missing equal");

        ast->value = parser_parse_expr(parser);
        tok_expected(parser, SEMI, "<var> <name>: <type> = <expr>; <- missing semicolon");

        char* size = calloc(1, sizeof(char));
        switch (ast->data_type)
        {
            case ptr: stack_push(parser->stack, ast->name, (stack_get_current_element_location(parser->stack) + 8)); break;
            case byte: { stack_push(parser->stack, ast->name, (stack_get_current_element_location(parser->stack) + 1)); size = "byte"; break; }
            case i16: { stack_push(parser->stack, ast->name, (stack_get_current_element_location(parser->stack) + 2)); size = "word"; break; }
            case i32: { stack_push(parser->stack, ast->name, (stack_get_current_element_location(parser->stack) + 4)); size = "dword"; break; }
        }

        char* template = "\tmov %s [ebp - %i], %i\n";
        char* src = calloc(strlen(template) + 128, sizeof(char));
        sprintf(src, template, size, stack_get_current_element_location(parser->stack), ast->value->int_value);

        parser->local_asm = realloc(parser->local_asm, (strlen(parser->local_asm) + strlen(src) + 128) * sizeof(char));
        strcat(parser->local_asm, src);

        return ast;
    }
    else err(0, "Statement only supports <var> assignments");
}

AST_T* parser_parse_compound(parser_T* parser)
{
    AST_T* compound = init_ast(AST_COMPOUND);
    while (parser->token->type != EOFL)
    {
        list_push(compound->body, parser_parse_statement(parser));
    }

    return compound;
}