#include "include/stack_frame.h"
#include "include/macro.h"

#include <string.h>

stack_T* init_stack()
{
    stack_T* stack = calloc(1, sizeof(struct STACK_STRUCT));
    stack->list = init_list(sizeof(struct key_pair));

    return stack;
}

void stack_push(stack_T* stack, char* name, int location)
{
    if(stack_get_location(stack, name))
    {
        char* template = "redeclaration of a variabel `%s`, cannot redeclare already declared variable.";
        char* src = calloc(strlen(template) + 8, sizeof(char));
        sprintf(src, template, name);
        err(0, src);
    }

    struct key_pair* new_pair = calloc(1, sizeof(struct key_pair));
    new_pair->name = name; new_pair->location = location;
    list_push(stack->list, new_pair);
}

size_t stack_get_size(stack_T* stack)
{
    return stack->list->size;
}

int stack_get_location(stack_T* stack, char* name)
{
    for (int i = 0; i < stack->list->size; i++)
    {
        struct key_pair* mp = (struct key_pair*)stack->list->items[i];
        if (!strcmp(name, mp->name)) return mp->location;
    }

    return 0;
}

list_T* stack_get_list(stack_T* stack)
{
    return stack->list;
}

void stack_print(stack_T* stack)
{
    for (int i = 0; i < stack->list->size; i++)
    {
        struct key_pair* mp = (struct key_pair*)stack->list->items[i];
        printf("[Stack]: <%s: %i>\n", mp->name, mp->location);
    }
}

struct key_pair* stack_get_current_element(stack_T* stack)
{
    if (stack->list->size == 0) return NULL;
    return (struct key_pair*)stack->list->items[stack->list->size - 1];
}

char* stack_get_current_element_name(stack_T* stack)
{
    struct key_pair* mp = stack_get_current_element(stack);
    if (mp) return mp->name;
    return NULL;
}

int stack_get_current_element_location(stack_T* stack)
{
    struct key_pair* mp = stack_get_current_element(stack);
    if (mp) return mp->location;
    return 0;
}
