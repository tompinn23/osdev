#pragma once

#include "stivale2.h"
#include <stddef.h>
#include "util/bitmap.h"
#include "util/linked.h"

#define MAX_ORDER 10

#define PAGE_SIZE 0x1000

#define BYTES_PER_BUCKET 16

typedef uint64_t pfn_t;

struct free_area {
    uint64_t phys_base;
    size_t len;
    struct list_header free_lists[MAX_ORDER];
    struct bitmap map[MAX_ORDER];
};

enum zone_type {
    ZONE_DMA,
    ZONE_DMA32,
    ZONE_NORMAL,
    ZONE_HIGHMEM,
    MAX_NR_ZONES
};

struct zone_area {
    pfn_t min_pfn;
    size_t len;

    //struct free_area
    
};

