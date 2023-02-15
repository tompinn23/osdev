#pragma once

#include <stddef.h>

typedef struct intrusive_list {
  struct intrusive_list *prev, *next;
} intrusive_list_t;

#define intrusive_list_container_of(ptr, sample, member)                       \
  (__typeof__(sample))((char *)(ptr)-offsetof(__typeof__(*sample), member))

/*
 * Initialize a list to empty. Because these are circular lists, an "empty"
 * list is an entry where both links point to itself. This makes insertion
 * and removal simpler because they don't need any branches.
 */
static inline void intrusive_list_init(intrusive_list_t *list) {
  list->prev = list;
  list->next = list;
}

/*
 * Append the provided entry to the end of the list. This assumes the entry
 * isn't in a list already because it overwrites the linked list pointers.
 */
static inline void intrusive_list_push(intrusive_list_t *list,
                                       intrusive_list_t *entry) {
  intrusive_list_t *prev = list->prev;
  entry->prev = prev;
  entry->next = list;
  prev->next = entry;
  list->prev = entry;
}

/*
 * Remove the provided entry from whichever list it's currently in. This
 * assumes that the entry is in a list. You don't need to provide the list
 * because the lists are circular, so the list's pointers will automatically
 * be updated if the first or last entries are removed.
 */
static inline void intrusive_list_remove(intrusive_list_t *entry) {
  intrusive_list_t *prev = entry->prev;
  intrusive_list_t *next = entry->next;
  prev->next = next;
  next->prev = prev;
}

/*
 * Remove and return the first entry in the list or NULL if the list is empty.
 */
static inline intrusive_list_t *intrusive_list_pop(intrusive_list_t *list) {
  intrusive_list_t *back = list->prev;
  if (back == list)
    return NULL;
  intrusive_list_remove(back);
  return back;
}
