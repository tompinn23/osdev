#include "mm/phys_mem.h"

#include "mm/memory.h"
#include "printf.h"
#include "util.h"


struct page* const page_data = NULL;


struct zone_area memory_zones[MAX_NR_ZONES];

/*
    bitmaps need 1 bit for the total of the following equation.
    x + 1/2x + 1/4x + 1/8x + 1/16x + 1/32x + 1/64x + 1/128x + 1/256x + 1/512x

    x + (0.5 * x) + (0.25 * x) + ((1/8) * x) + ((1/16) * x) + ((1/32) * x) + ((1/64) * x) + ((1/128) * x) + ((1/256) * x) + ((1/512) * x)
*/

uint64_t get_available_pages(struct free_area area) {
//    uint64_t effective_start = ALIGNUP(area.phys_base + sizeof(struct free_area), PAGE_SIZE);
 //   uint64_t effective_len = area.len - (effective_start - area.phys_base);
  //  uint64_t pages = effective_len / (PAGE_SIZE);
    //uint64_t bitmap_size = DIV_ROUND_UP()
   // printf("big_bucket: %d, pages_for_bitmap: %d\n", big_bucket_count, pages_for_map);
}


/**
 * Initialize the zoned buddy allocator
 * 
 * 
 */

void pmm_init(struct page* page_data, struct stivale2_struct_tag_memmap* mem_map) {
	if(page_data == NULL) {
		panic("bootmem needs to be initialized before phyisical memory");
	}
	size_t idx = 0;
	int dma_area, normal_area, highmem_area;
	pfn_t dma_min, normal_min, highmem_min;
	dma_min = normal_min = highmem_min = 0xFFFFFFFFFFFFFFFF;
	dma_area = normal_area = highmem_area = 0;
	for(idx = 0; idx < mem_map->entries; idx++) {
		struct stivale2_mmap_entry entry = mem_map->memmap[idx];
		uint64_t sz = entry.base + entry.length;
		switch(entry.type) {
		case STIVALE2_MMAP_USABLE:
		case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
			if(sz < ZONE_DMA_SZ) {
				if(entry.base < dma_min) {
					dma_min = entry.base;
				}
				dma_area += 1;
			}
			else if(sz < ZONE_NORMAL_SZ) {
				if(entry.base < normal_min) {
					normal_min = entry.base;
				}
				normal_area += 1;
			}
			else {
				if(entry.base < highmem_min) {
					highmem_min = entry.base;
				}
				highmem_area += 1;
			}
		break;
		default:
		break;
		}
	}
	memory_zones[ZONE_DMA].free_areas = bootmem_alloc((sizeof(struct free_area) * dma_area) / PAGE_SIZE);
	memory_zones[ZONE_DMA].min_pfn = dma_min;
	memory_zones[ZONE_NORMAL].free_areas = bootmem_alloc((sizeof(struct free_area) * dma_area) / PAGE_SIZE);
	memory_zones[ZONE_NORMAL].min_pfn = normal_min;
	memory_zones[ZONE_HIGHMEM].free_areas = bootmem_alloc((sizeof(struct free_area) * dma_area) / PAGE_SIZE);
	memory_zones[ZONE_HIGHMEM].min_pfn = highmem_min;

}
