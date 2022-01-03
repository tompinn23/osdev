#pragma once

#include "stivale2.h"
#include <stddef.h>
#include "util/bitmap.h"
#include "util/linked.h"

#define MAX_ORDER 10

#define PAGE_SIZE 0x1000

#define BYTES_PER_BUCKET 16

#define ZONE_DMA_SZ 0x1000000
#define ZONE_NORMAL_SZ 0x00000000ffffffff

typedef uint64_t pfn_t;
extern uint64_t _kernel_end_phys[];

struct page {
    struct {
        struct list_node list;
        int8_t order;
    };
};

extern struct page* const page_data;

enum zone_type {
    ZONE_DMA,
//    ZONE_DMA32,
    ZONE_NORMAL,
    ZONE_HIGHMEM,
    MAX_NR_ZONES
};

struct free_area {
    pfn_t min_pfn;
    size_t len;
    struct list_node* free_lists[MAX_ORDER];
    bitmap_t free_map[MAX_ORDER];
};

struct zone_area {
    pfn_t min_pfn;
    size_t len;
    struct frea_area* free_areas;
};

