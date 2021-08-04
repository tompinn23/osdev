#include "mm/phys_mem.h"

#include "mm/memory.h"
#include "printf.h"
#include "util.h"


/*
    bitmaps need 1 bit for the total of the following equation.
    x + 1/2x + 1/4x + 1/8x + 1/16x + 1/32x + 1/64x + 1/128x + 1/256x + 1/512x

    x + (0.5 * x) + (0.25 * x) + ((1/8) * x) + ((1/16) * x) + ((1/32) * x) + ((1/64) * x) + ((1/128) * x) + ((1/256) * x) + ((1/512) * x)
*/

uint64_t get_available_pages(struct free_area area) {
    uint64_t effective_start = ALIGNUP(area.phys_base + sizeof(struct free_area), PAGE_SIZE);
    uint64_t effective_len = area.len - (effective_start - area.phys_base);
    uint64_t pages = effective_len / (PAGE_SIZE);
    //uint64_t bitmap_size = DIV_ROUND_UP()
   // printf("big_bucket: %d, pages_for_bitmap: %d\n", big_bucket_count, pages_for_map);
}




void pmm_init(struct stivale2_struct_tag_memmap* memory_map) {
    for(size_t idx = 0; idx < memory_map->entries; idx++) {
        struct stivale2_mmap_entry entry = memory_map->memmap[idx];
        if(entry.type == STIVALE2_MMAP_USABLE) {
            struct free_area area;
            area.phys_base = entry.base;
            area.len = entry.length;
            if(get_available_pages(area) > 0) { // usable memory area
            
            }
        }
    }
}
