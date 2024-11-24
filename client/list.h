#pragma once

#include <stddef.h>

typedef struct {
    void* items;
    unsigned length;
    unsigned capacity;
} List;

void append_to_list(List* list, void* item, size_t item_size);

void dispose_list(List* list);