#include "buddy.h"

#include "libk/kutil.h"
#include "log.h"
#include <stddef.h>

static inline void map_set(uint64_t *set, size_t i) {
  set[i >> 6] |= ((uint64_t)1) << (i % 64);
}

static inline void __change_bit(long nr, volatile unsigned long *addr) {
  __asm__ volatile("btcq %1,%0"
                   :
                   : "m"(*(volatile long *)(addr)), "Ir"(nr)
                   : "memory");
}

/*
 * Initialize a list to empty. Because these are circular lists, an "empty"
 * list is an entry where both links point to itself. This makes insertion
 * and removal simpler because they don't need any branches.
 */
static void list_init(list_t *list) {
  list->prev = list;
  list->next = list;
}

/*
 * Append the provided entry to the end of the list. This assumes the entry
 * isn't in a list already because it overwrites the linked list pointers.
 */
static void list_push(list_t *list, list_t *entry) {
  list_t *prev = list->prev;
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
static void list_remove(list_t *entry) {
  list_t *prev = entry->prev;
  list_t *next = entry->next;
  prev->next = next;
  next->prev = prev;
}

/*
 * Remove and return the first entry in the list or NULL if the list is empty.
 */
static list_t *list_pop(list_t *list) {
  list_t *back = list->prev;
  if (back == list)
    return NULL;
  list_remove(back);
  return back;
}

static list_t *list_check(list_t *list) {
  list_t *back = list->prev;
  if (back == list)
    return NULL;
  return back;
}

static int64_t iDivCeil(int64_t x, int64_t y) { return (x + (y - 1)) / y; }

static size_t get_index_for_ptr(buddy_t *b, void *ptr, int order) {
  if (order >= BUDDY_MAX_LEVEL) {
    return 0;
  }
  return (uint64_t)(ptr - b->base) / ((1 << (order + 1)) * PAGE_SIZE);
}

static void *get_ptr_for_index(buddy_t *b, int index, int order) {
  if (order >= BUDDY_MAX_LEVEL) {
    return 0;
  }
  return b->base + (index * ((1 << (order + 1)) * PAGE_SIZE));
}

/* CAVEAT: this function shouldn't be used once the allocator is allocating */
static void __buddy_lock_page(buddy_t *bud, size_t page) {
  void *addr = get_ptr_for_index(bud, page, 0);
  for (int i = 0; i < BUDDY_MAX_LEVEL; i++) {
    __change_bit(get_index_for_ptr(bud, addr, i), bud->free_area[i].map);
  }
}
buddy_t *buddy_init(int zone_ty, uint64_t base, uint64_t length) {
  buddy_t *bd = (void *)base;
  memzero(bd, sizeof(*bd));
  bd->bud_magic = 0xdeadbeef;
  bd->bud_magic2 = 0xbeefdead;
  bd->zone_type = zone_ty;
  bd->base = (void *)base;
  bd->length = length;
  int divisor = PAGE_SIZE;
  void *map_base = (void *)(base + sizeof(buddy_t));
  for (int i = 1; i <= BUDDY_MAX_LEVEL; i++) {
    if (i == 1) {
      list_init(&bd->free_area[i - 1].free_list);
    } else {
      list_init(&bd->free_area[i - 1].free_list);
      bd->free_area[i - 1].map = map_base;
      map_base += iDivCeil(length, divisor * 2);
      divisor *= 2;
    }
  }
  size_t map_length = map_base - (void *)(base);
  dbg("buddy:: map_length (PAGES) %d\n", iDivCeil(map_length, PAGE_SIZE));
  bd->reserved_pages = iDivCeil(map_length, PAGE_SIZE);
  bd->base += bd->reserved_pages * PAGE_SIZE;
  bd->length -= bd->reserved_pages * PAGE_SIZE;
  // TODO: handle non 512k blocks.
  for (int i = BUDDY_MAX_LEVEL - 1; i > 0; i--) {
    int64_t top_level_free = iDivCeil(bd->length, ((1 << i) * PAGE_SIZE));
    if (top_level_free > 0) {
      for (int64_t j = 0; j < top_level_free; j++) {
        void *addr = get_ptr_for_index(bd, j, i - 1);
        dbg("buddy :: add_free: 0x%p\n", addr);
        list_push(&bd->free_area[i].free_list, addr);
      }
      break;
    }
  }
  return bd;
}

void *buddy_split_block(buddy_t *buddy, int order, int requested_order) {
  if (order == 0) /* You can't split a page dummy */
    return NULL;
  do {
    void *free = list_pop(&buddy->free_area[order].free_list);
    uint64_t bud_size = ((1 << (order - 1)) * PAGE_SIZE);
    void *free_buddy = free + bud_size;
    list_push(&buddy->free_area[order - 1].free_list, free_buddy);
    list_push(&buddy->free_area[order - 1].free_list, free);
    // flip bit of (current order index) parent of the two new blocks.
    __change_bit(get_index_for_ptr(buddy, free, order),
                 buddy->free_area[order].map);
    dbg("buddy :: splitting block of order: %d at: 0x%p buddy: 0x%p\n", order,
        free, free_buddy);
    order--;
  } while (order != requested_order && order != 0);
  dbg("buddy :: req_order: %d\n", requested_order);
  list_t *free = list_pop(&buddy->free_area[requested_order].free_list);
  if (free != NULL) {
    __change_bit(get_index_for_ptr(buddy, free, requested_order),
                 buddy->free_area[requested_order + 1].map);
  }
  return free;
}

/*
   Iterate up the block sizes 4k -> 512k
   if order == 0 && free
    return free page

  otherwise we've got a free block but its too big
  split

*/
void *buddy_alloc_page(buddy_t *buddy) {
  for (int order = 0; order < BUDDY_MAX_LEVEL; order++) {
    list_t *free = list_check(&buddy->free_area[order].free_list);
    if (order == 0 && free) {
      list_remove(free);
      return free;
    }
    if (free) {
      return buddy_split_block(buddy, order, 0);
    }
  }
  return NULL;
}
