#include "include/list.h"
#include "include/macro.h"

list_T* init_list(size_t item_size)
{
    list_T* list = calloc(1, sizeof(struct LIST_STRUCT));
    list->size = 0;
    list->item_size = item_size;
    list->items = 0;

    return list;
}

void list_push(list_T* list, void* item)
{
    list->size++;
    if (!list->items)
        list->items = calloc(1, list->item_size);
    else
        list->items = realloc(list->items, (list->size * list->item_size));

    list->items[list->size-1] = item;
}

void* list_pop_first(list_T* list)
{
    if (!list->size) return NULL;

    void* item = list->items[0];

    if (list->size != 1) for (int i = 0; i < list->size; i++) list->items[i] = list->items[i + 1];
    else list->items[0] = NULL;

    list->size--; list->items = realloc(list->items, (list->size * list->item_size));

    return item;
}

map_T* init_map()
{
    map_T* map = calloc(1, sizeof(struct MAP_STRUCT));
    map->list = init_list(sizeof(struct map_pair));
    return map;
}

void map_push(map_T* map, void* key, void* value)
{
    if (map_get(map, key)) err(0, "Cannot assign pair, it's already been assigned.");
    struct map_pair* new_pair = calloc(1, sizeof(struct map_pair));
    new_pair->key = key; new_pair->value = value;
    list_push(map->list, new_pair);
}

void* map_get(map_T* map, void* key)
{
    for (int i = 0; i < map->list->size; i++)
    {
        struct map_pair* mp = (struct map_pair*)map->list->items[i];
        if (mp->key == key) return mp->value;
    }

    return NULL;
}

