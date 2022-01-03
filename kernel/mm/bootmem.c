#include "mm/bootmem.h"

#include <util.h>
#include <string.h>
#include <printf.h>

#if defined(DEBUG_early_pmm) || defined(DEBUG_ALL)
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

void correct_memmap(struct stivale2_struct_tag_memmap* map, size_t idx, uint64_t* base, uint64_t* length) {
	struct stivale2_mmap_entry* entry = &map->memmap[idx];
	struct stivale2_mmap_entry* prev = idx > 0 ? &map->memmap[idx - 1] : NULL;
	struct stivale2_mmap_entry* next = idx + 1 < map->entries ? &map->memmap[idx + 1] : NULL;
	*base = entry->base;
	*length = entry->length;
//	dbg("base: %x prev base: %x prev end: %x\n", entry->base, prev->base, prev->base + prev->length);
	if(prev != NULL && (entry->base >= prev->base && entry->base <= prev->base + prev->length)) {
		uint64_t diff = (prev->base + prev->length) - entry->base;
//		dbg("correcting: b %x -> %x\n", *base, *base + diff); 
		*base += diff;
		*length -= diff;	
	}
	if(next != NULL && (entry->base + entry->length >= next->base)) {
		*length -= (entry->base + entry->length) - next->base;
	}
}


void bootmem_init(struct page** page_dat, struct stivale2_struct_tag_memmap* mem_map) {
    uint64_t pages = 0;
    for(size_t idx = 0; idx < mem_map->entries; idx++) {
        struct stivale2_mmap_entry entry = mem_map->memmap[idx];
        if(entry.type == STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE || entry.type == STIVALE2_MMAP_ACPI_RECLAIMABLE) {
            bootmem_add_reserved(entry.base, entry.length);
            pages += entry.length / PAGE_SIZE;
        } else if(entry.type == STIVALE2_MMAP_USABLE) {
			uint64_t base, length;
			correct_memmap(mem_map, idx, &base, &length);
            bootmem_add_usable(base, length);
            pages += length / PAGE_SIZE;
        }
    }
  //  dbg("Number of pages: %d\n", pages);
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
void* bootmem_alloc(int num) {
        // This should only be called if the function above was run, but not after
    // the virtual memory is online either
	if(num == 0) 
		return NULL;
    //dbg("Alloc %d pages\n", num);
	if(init_mem.reserved.count >= init_mem.reserved.max) {
		panic("early_alloc ran out of memblocks to reserve");
		return NULL;
	}

    unsigned i;
    for(i = 0; i < init_mem.memory.count; i++) {
//		dbg("testing for alloc: 0x%p - %d\n", init_mem.memory.regions[i].base, init_mem.memory.regions[i].size);
        if(init_mem.memory.regions[i].size <= PAGE_SIZE ||
           init_mem.memory.regions[i].base >= 0x100000000ULL) {
            // Throw away pages that exist over 4GB for now, or tinier than 4k.
            continue;
        }
        // under the kernel? Ignore it!
        if(init_mem.memory.regions[i].base <= (uint64_t)_kernel_end_phys) {
//			dbg("discarding: %x\n", init_mem.memory.regions[i].base);
            continue;
        }
        if(init_mem.memory.regions[i].size < (uint64_t)num * PAGE_SIZE) {
//          dbg("size: %d\n", init_mem.memory.regions[i].size);
            continue;
        }
//		dbg("allocating at: %x\n", init_mem.memory.regions[i].base);
        uint64_t alloc_base = init_mem.memory.regions[i].base;
        init_mem.memory.regions[i].base  += num * PAGE_SIZE;
        init_mem.memory.regions[i].size  -= num * PAGE_SIZE;
		for(unsigned j = 0; j < init_mem.reserved.count; j++) {
			struct memblock_region region = init_mem.reserved.regions[j];
			uint64_t alloc_end = alloc_base + (num * PAGE_SIZE);
			uint64_t reg_end = region.base + region.size;
			// if reserved region touches newly allocated concatenate the regions.
			if(alloc_end == region.base) {
//				dbg("concatenating reserved regions n -> o");
				region.size += num * PAGE_SIZE;
				region.base = alloc_base;
				init_mem.reserved.regions[j] = region;
				break;
			}
			else if(reg_end == alloc_base) {
//				dbg("concatenating reserved regions o -> n");
				region.size += num * PAGE_SIZE;
				init_mem.reserved.regions[j] = region;
				break;
			}
			else {
//				dbg("allocating new reserved region");
				if(init_mem.reserved.count >= init_mem.reserved.max) {
					dbg("early_alloc ran out of reserved region space: %d/%d", init_mem.reserved.count, init_mem.reserved.max);
					panic("early_alloc ran out of reserved region space");
					return NULL;
				}
				struct memblock_region newly_reserved;
				newly_reserved.base = alloc_base;
				newly_reserved.size = num * PAGE_SIZE;
				init_mem.reserved.regions[init_mem.reserved.count++] = newly_reserved;
				break;
			}
		}
        dbg("early_alloc_page() -> %x\n", alloc_base);
        return (uint64_t*)alloc_base;
    }
    panic("Early_alloc_page couldn't find any pages to use!");
	return NULL;
}

void bootmem_free_early(uint64_t base, uint64_t len) {
}


