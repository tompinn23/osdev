#pragma once

#include <stdint.h>

#define MEMBLOCK_INIT_REGIONS 128

struct memblock_region {
    uint64_t base;
    uint64_t size;

};

struct memblock_type
{
    uint64_t count;
    uint64_t max;
    uint64_t total_size;
    struct memblock_region* regions;
};


struct bootmem_data {
    uint64_t current_limit;
    struct memblock_type memory;
    struct memblock_type reserved;
};

//bootmem_init(pg_data_t* page_data, uint64_t map_start, uint64_t start, uint64_t end);


void* bootmem_alloc();

void bootmem_free_early(uint64_t base, uint64_t len);