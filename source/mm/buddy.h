#pragma once

#include "libk/intrusive_list.h"
#include <stdint.h>
enum { ZONE_DMA, ZONE_LOWMEM, ZONE_HIGHMEM };

#define BUDDY_MAX_LEVEL 8
#define PAGE_SIZE 4096

typedef struct list {
  struct list *next, *prev;
} list_t;

typedef struct free_area {
  struct list free_list;
  unsigned long *map;
} free_area_t;

typedef struct buddy {
  uint32_t bud_magic;
  /* Bitmaps from 4k to 512k */
  free_area_t free_area[BUDDY_MAX_LEVEL];
  void *base;
  size_t reserved_pages;
  uint64_t length;
  int zone_type;
  void *prev, *next;
  intrusive_list_t buddies;
  uint32_t bud_magic2;
} buddy_t;

buddy_t *buddy_init(int zone_ty, uint64_t base, uint64_t length);
void *buddy_alloc_page(buddy_t *b);
