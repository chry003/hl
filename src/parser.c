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
        char* str = calloc(strlen(template) + 128, sizeof(char));
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
        AST_T* ast = init_ast(AST_INT);
        ast->int_value = atoi(parser->token->value);
        parser_eat(parser, INT);

        return ast;
    }
    else if (parser->token->type == ID)
    {
        AST_T* ast = init_ast(AST_VARIABLE);

        char* name = NULL;
        while (parser->token->type == ID || parser->token->type == INT || parser->token->type == UNDERSCORE)
        {
            if (!name) name = calloc(1, sizeof(char));
            name = realloc(name, (strlen(name) + 128) * sizeof(char));
            strcat(name, parser->token->value);
            parser_eat(parser, parser->token->type);
        }

        ast->name = calloc(strlen(name), sizeof(char));
        strcpy(ast->name, name);

        int size = stack_get_location_with_index(parser->stack, stack_get_index(parser->stack, name)) - stack_get_location_with_index(parser->stack, stack_get_index(parser->stack, name) - 1);

        switch(size)
        {
            case 1: ast->data_type = byte; break;
            case 2: ast->data_type = i16; break;
            case 4: ast->data_type = i32; break;
            case 8: ast->data_type = ptr; break;
        }

        ast->int_value = stack_get_location(parser->stack, name);

        return ast;
    }
    else if (parser->token->type == LPAREN)
    {
        AST_T* ast = init_ast(AST_COMPOUND);

        parser_eat(parser, LPAREN);

        AST_T* lvalue = init_ast(AST_CONDITION);
        AST_T* condition = init_ast(AST_CONDITION); // != -> 0, == -> 1, > -> 2, < -> 3, <= -> 4, >= -> 5
        AST_T* rvalue = init_ast(AST_CONDITION);

        condition->data_type = i32;

        if (parser->token->type == ID)
        {
            char* name = NULL;
            while (parser->token->type == ID || parser->token->type == INT || parser->token->type == UNDERSCORE)
            {
                if (!name) name = calloc(1, sizeof(char));
                name = realloc(name, (strlen(name) + 128) * sizeof(char));
                strcat(name, parser->token->value);
                parser_eat(parser, parser->token->type);
            }

            int size = stack_get_location_with_index(parser->stack, stack_get_index(parser->stack, name)) - stack_get_location_with_index(parser->stack, stack_get_index(parser->stack, name) - 1);

            switch(size)
            {
                case 1: lvalue->data_type = byte; break;
                case 2: lvalue->data_type = i16; break;
                case 4: lvalue->data_type = i32; break;
                case 8: lvalue->data_type = ptr; break;
                default: err(0, "not supported");
            }

            lvalue->name = name;
            lvalue->int_value = stack_get_location(parser->stack, name);
        }
        else if (parser->token->type == INT)
        {
            lvalue->data_type = i32;
            lvalue->int_value = atoi(parser->token->value);
            parser_eat(parser, INT);
        }
        else err(0, "Only TOK_ID & TOK_INT are supported in <if> statement.");



        if (parser->token->type == EQUAL)
        {
            parser_eat(parser, EQUAL);
            tok_expected(parser, EQUAL, "if ( <expr> = <- missing equal, did you mean `==` this?");
            condition->int_value = 1;
        }
        else if (parser->token->type == LT)
        {
            parser_eat(parser, LT);
            condition->int_value = 2;
            if (parser->token->type == EQUAL)
            {
                parser_eat(parser, EQUAL);
                condition->int_value = 5;
            }
        }
        else if (parser->token->type == GT)
        {
            parser_eat(parser, GT);
            condition->int_value = 3;
            if (parser->token->type == EQUAL)
            {
                parser_eat(parser, EQUAL);
                condition->int_value = 4;
            }
        }



        if (parser->token->type == ID)
        {
            char* name = NULL;
            while (parser->token->type == ID || parser->token->type == INT || parser->token->type == UNDERSCORE)
            {
                if (!name) name = calloc(1, sizeof(char));
                name = realloc(name, (strlen(name) + 128) * sizeof(char));
                strcat(name, parser->token->value);
                parser_eat(parser, parser->token->type);
            }

            int size = stack_get_location_with_index(parser->stack, stack_get_index(parser->stack, name)) - stack_get_location_with_index(parser->stack, stack_get_index(parser->stack, name) - 1);

            switch(size)
            {
                case 1: rvalue->data_type = byte; break;
                case 2: rvalue->data_type = i16; break;
                case 4: rvalue->data_type = i32; break;
                case 8: rvalue->data_type = ptr; break;
                default: err(0, "not supported");
            }

            rvalue->name = name;
            rvalue->int_value = stack_get_location(parser->stack, name);
        }
        else if (parser->token->type == INT)
        {
            rvalue->data_type = lvalue->data_type;
            rvalue->int_value = atoi(parser->token->value);
            parser_eat(parser, INT);
        }
        else err(0, "Only TOK_ID & TOK_INT are supported in <if> statement.");

        tok_expected(parser, RPAREN, "Unexpected token type, expected if (condition) <- missing RPAREN");

        list_push(ast->body, lvalue); list_push(ast->body, condition); list_push(ast->body, rvalue);

        return ast;
    }
    else err(0, "Expr only supports TOK_TYPE: INT || ID");
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

        switch (ast->data_type)
        {
            case ptr: stack_push(parser->stack, ast->name, (stack_get_current_element_location(parser->stack) + 8)); break;
            case byte: { stack_push(parser->stack, ast->name, (stack_get_current_element_location(parser->stack) + 1)); break; }
            case i16: { stack_push(parser->stack, ast->name, (stack_get_current_element_location(parser->stack) + 2)); break; }
            case i32: { stack_push(parser->stack, ast->name, (stack_get_current_element_location(parser->stack) + 4)); break; }
        }

        ast->value = parser_parse_expr(parser);
        tok_expected(parser, SEMI, "<var> <name>: <type> = <expr>; <- missing semicolon");

        return ast;
    }
    else if (tok_value_match(parser, "if"))
    {
        parser_eat(parser, ID);
        tok_expected(parser, LPAREN, "<if> statement needs a condition enclosed with parenthesis.");

        AST_T* lvalue = init_ast(AST_CONDITION);
        AST_T* condition = init_ast(AST_CONDITION); // != -> 0, == -> 1, > -> 2, < -> 3, <= -> 4, >= -> 5
        AST_T* rvalue = init_ast(AST_CONDITION);

        condition->data_type = i32;

        if (parser->token->type == ID)
        {
            char* name = NULL;
            while (parser->token->type == ID || parser->token->type == INT || parser->token->type == UNDERSCORE)
            {
                if (!name) name = calloc(1, sizeof(char));
                name = realloc(name, (strlen(name) + 128) * sizeof(char));
                strcat(name, parser->token->value);
                parser_eat(parser, parser->token->type);
            }

            int size = stack_get_location_with_index(parser->stack, stack_get_index(parser->stack, name)) - stack_get_location_with_index(parser->stack, stack_get_index(parser->stack, name) - 1);

            switch(size)
            {
                case 1: lvalue->data_type = byte; break;
                case 2: lvalue->data_type = i16; break;
                case 4: lvalue->data_type = i32; break;
                case 8: lvalue->data_type = ptr; break;
            }

            lvalue->name = name;
            lvalue->int_value = stack_get_location(parser->stack, name);
        }
        else if (parser->token->type == INT)
        {
            lvalue->data_type = i32;
            lvalue->int_value = atoi(parser->token->value);
            parser_eat(parser, INT);
        }
        else err(0, "Only TOK_ID & TOK_INT are supported in <if> statement.");

        if (parser->token->type == EQUAL)
        {
            parser_eat(parser, EQUAL);
            tok_expected(parser, EQUAL, "if ( <expr> = <- missing equal, did you mean `==` this?");
            condition->int_value = 1;
        }

        if (parser->token->type == ID)
        {
            char* name = NULL;
            while (parser->token->type == ID || parser->token->type == INT || parser->token->type == UNDERSCORE)
            {
                if (!name) name = calloc(1, sizeof(char));
                name = realloc(name, (strlen(name) + 128) * sizeof(char));
                strcat(name, parser->token->value);
                parser_eat(parser, parser->token->type);
            }

            int size = stack_get_location_with_index(parser->stack, stack_get_index(parser->stack, name)) - stack_get_location_with_index(parser->stack, stack_get_index(parser->stack, name) - 1);

            switch(size)
            {
                case 1: rvalue->data_type = byte; break;
                case 2: rvalue->data_type = i16; break;
                case 4: rvalue->data_type = i32; break;
                case 8: rvalue->data_type = ptr; break;
            }

            rvalue->name = name;
            rvalue->int_value = stack_get_location(parser->stack, name);
        }
        else if (parser->token->type == INT)
        {
            rvalue->data_type = lvalue->data_type;
            rvalue->int_value = atoi(parser->token->value);
            parser_eat(parser, INT);
        }
        else err(0, "Only TOK_ID & TOK_INT are supported in <if> statement.");

        tok_expected(parser, RPAREN, "Unexpected token type, expected if (condition) <- missing RPAREN");

        ast->type = AST_IF;
        ast->body = init_list(sizeof(struct AST_STRUCT));
        list_push(ast->body, lvalue); list_push(ast->body, condition); list_push(ast->body, rvalue);

        return ast;
    }
    else if (parser->token->type == ID)
    {
        char* name = NULL;
        while (parser->token->type == ID || parser->token->type == INT || parser->token->type == UNDERSCORE)
        {
            if (!name) name = calloc(1, sizeof(char));
            name = realloc(name, (strlen(name) + 128) * sizeof(char));
            strcat(name, parser->token->value);
            parser_eat(parser, parser->token->type);
        }

        int size = stack_get_location_with_index(parser->stack, stack_get_index(parser->stack, name)) - stack_get_location_with_index(parser->stack, stack_get_index(parser->stack, name) - 1);

        switch(size)
        {
            case 1: ast->data_type = byte; break;
            case 2: ast->data_type = i16; break;
            case 4: ast->data_type = i32; break;
            case 8: ast->data_type = ptr; break;
        }

        if (stack_get_location(parser->stack, name))
        {
            ast->type = AST_VARIABLE_ASSIGN;
            ast->name = calloc(1, sizeof(char));
            strcpy(ast->name, name);

            tok_expected(parser, EQUAL, "<var_name> = <- missing equal");
            ast->value = parser_parse_expr(parser);

            tok_expected(parser, SEMI, "<var_name> = <expr>; <- missing semicolon");
        }
        else err(0, "cannot assign <expr> to unknown variable");

        return ast;
    }
    else
    {
        char* template = "Unexpected <statement>, did you mean to use this? `%s`";
        char* src = calloc(strlen(template) + 128, sizeof(char));
        sprintf(src, template, token_to_str(parser->token));
        err(0, src);
    }
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
