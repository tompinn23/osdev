#include "mm/phys_mem.h"

#include "mm/memory.h"


static struct mem_stuff {
    uint64_t normal_base;
} mem_data;


void pmm_init(struct stivale2_struct_tag_memmap* memory_map) {

}
