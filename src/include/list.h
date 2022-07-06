#pragma once

#include <stdlib.h>

typedef struct LIST_STRUCT
{
    void** items;
    size_t size;
    size_t item_size;
} list_T;

list_T* init_list(size_t item_size);
void list_push(list_T* list, void* item);

typedef struct MAP_STRUCT
{
    list_T* list;
} map_T;

struct map_pair
{
    void* key;
    void* value;
};

map_T* init_map();
void map_push(map_T* map, void* key, void* value);
void* map_get(map_T* map, void* key);
