#include "list.h"

#include <stdlib.h>
#include <string.h>

void append_to_list(List *list, void *item, size_t item_size) {
  if (list->capacity == 0) {
    list->items = malloc(2 * item_size);
    list->capacity = 2;
  } else if (list->length == list->capacity) {
    void *old = list->items;
    list->items = malloc(2 * (list->capacity) * item_size);
    memcpy(list->items, old, list->capacity * item_size);
    list->capacity = 2 * list->capacity;
    free(old);
  }
  memcpy((char *)list->items + (list->length * item_size), item, item_size);
  list->length++;
}

void dispose_list(List *list) {
  free(list->items);
  memset(list, 0, sizeof(List));
}