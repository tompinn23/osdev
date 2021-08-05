#include "mm/bootmem.h"

struct memblock_region memory[MEMBLOCK_INIT_REGIONS];
struct memblock_region reserved[MEMBLOCK_INIT_REGIONS];

struct bootmem_data init_mem = {
    .current_limit = 0,
    .memory.regions = memory,
    .memory.count = 0,
    .memory.max = MEMBLOCK_INIT_REGIONS,

    .reserved.regions = reserved,
    .reserved.count = 0,
    .reserved.max = MEMBLOCK_INIT_REGIONS   
};

void memblock_add_region(uint64_t base, uint64_t len) {
    
}