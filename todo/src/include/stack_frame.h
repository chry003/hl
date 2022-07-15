#pragma once

#include "list.h"

typedef struct STACK_STRUCT
{
    list_T* list;
} stack_T;

struct key_pair
{
    char* name;
    int location;
};

stack_T* init_stack();

void stack_push(stack_T* stack, char* name, int location);

size_t stack_get_size(stack_T* stack);

int stack_get_location(stack_T* stack, char* name);

list_T* stack_get_list(stack_T* stack);

void stack_print(stack_T* stack);

struct key_pair* stack_get_current_element(stack_T* stack);

char* stack_get_current_element_name(stack_T* stack);

int stack_get_current_element_location(stack_T* stack);

int stack_peek_element_location(stack_T* stack, int offset);

int stack_get_index(stack_T* stack, char* name);

int stack_get_location_with_index(stack_T* stack, int index);
