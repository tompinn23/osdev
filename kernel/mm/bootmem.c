#include "mm/bootmem.h"

#include <util.h>
#include <string.h>
#include <printf.h>

#ifdef DEBUG_early_pmm
# define dbg(args...) kprintf("DBG//early_pmm: " args)
#else
# define dbg(args...)
#endif


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

void bootmem_init(struct page** page_dat, struct stivale2_struct_tag_memmap* mem_map) {
    uint64_t pages = 0;
    for(size_t idx = 0; idx < mem_map->entries; idx++) {
        struct stivale2_mmap_entry entry = mem_map->memmap[idx];
        if(entry.type == STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE || entry.type == STIVALE2_MMAP_ACPI_RECLAIMABLE) {
            bootmem_add_reserved(entry.base, entry.length);
            pages += entry.length / PAGE_SIZE;
        } else if(entry.type == STIVALE2_MMAP_USABLE) {
            bootmem_add_usable(entry.base, entry.length);
            pages += entry.length / PAGE_SIZE;
        }
    }
    dbg("Number of pages: %d\n", pages);
    int alloc_pages = (pages * sizeof(struct page)) / PAGE_SIZE;
    *page_dat = (void*)bootmem_alloc(alloc_pages);
    //memzero(page_dat, alloc_pages * PAGE_SIZE);
}

void bootmem_add_reserved(uint64_t base, uint64_t length) {
    bootmem_add(&init_mem.reserved, base, length);
}

void bootmem_add_usable(uint64_t base, uint64_t length) {
    bootmem_add(&init_mem.memory, base, length);
}

void bootmem_add(struct memblock_type* mem, uint64_t base, uint64_t length) {
    if(mem->count < mem->max) {
        mem->regions[mem->count].base = base;
        mem->regions[mem->count].size = length;
        mem->count += 1;
    } else {
        kprintf("Failed to add memblock: %d, base: %d, length: %d", mem->count, base, length);
    }
}
uint64_t bootmem_alloc(int num) {
        // This should only be called if the function above was run, but not after
    // the virtual memory is online either
    dbg("Alloc %d pages\n", num);
    unsigned i;
    for(i = 0; i < init_mem.memory.max; i++) {
        if(init_mem.memory.regions[i].size <= PAGE_SIZE ||
           init_mem.memory.regions[i].base >= 0x100000000ULL) {
            // Throw away pages that exist over 4GB for now, or tinier than 4k.
            continue;
        }
        // under the kernel? Ignore it!
        if(init_mem.memory.regions[i].base <= *_kernel_end_phys) {
            continue;
        }
        if(init_mem.memory.regions[i].size < num * PAGE_SIZE) {
            dbg("size: %d\n", init_mem.memory.regions[i].size);
            continue;
        }
        uint64_t ret = init_mem.memory.regions[i].base;
        init_mem.memory.regions[i].base  += num * PAGE_SIZE;
        init_mem.memory.regions[i].size  -= num * PAGE_SIZE;

        dbg("early_alloc_page() -> %x\n", ret);
        return ret;
    }
    panic("Early_alloc_page couldn't find any pages to use!");
}