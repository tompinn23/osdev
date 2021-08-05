#include <stdint.h>
#include <stddef.h>

#include "stivale2.h"
#include "mm/memory.h"
#include "console/console.h"
#include "printf.h"
#include "cpu/idt.h"
#include "isr.h"

// tell the stivale bootloader where we want our stack.
static uint8_t stack[4096];


static struct stivale2_header_tag_framebuffer framebuffer_hdr_tag = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = 0
    },
    .framebuffer_width = 0,
    .framebuffer_height = 0,
    .framebuffer_bpp = 0
};

__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header  stivale_header = {
    .entry_point = 0,
    .stack = (uintptr_t)stack + sizeof(stack),
    .flags = (1 << 1) | (1 << 2),
    .tags = (uintptr_t)&framebuffer_hdr_tag
};

void* stivale2_get_tag(struct stivale2_struct* stivale2_struct, uint64_t id) {
    struct stivale2_tag* cur_tag = (void*)stivale2_struct->tags;
    for(;;) {
        if(cur_tag == NULL) {
            return NULL;
        }

        if(cur_tag->identifier == id) {
            return cur_tag;
        }

        cur_tag = (void*)cur_tag->next;
    }
}


extern uint64_t _kernel_end_phys[];

void kmain(struct stivale2_struct* stivale2_struct) {
    struct stivale2_struct_tag_framebuffer* term_str_tag;
    term_str_tag = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    if(term_str_tag == NULL) {
        for(;;) {
            asm ("hlt");
        }
    }

    struct stivale2_struct_tag_memmap* mem_map;
    mem_map = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_MEMMAP_ID);

    init_console(term_str_tag);

    int i = 0;
    for(i = 0; i < mem_map->entries; i++)
        if(mem_map->memmap[i].type == STIVALE2_MMAP_USABLE) {
            break;
        }


    //bootmem_init(pg_data_t* page_data, uint64_t map_start, uint64_t start, uint64_t end);

    pmm_init(mem_map);
    
    kprintf("Kernel End: 0x%p\n", _kernel_end_phys);

    uint64_t avail_total = 0;
    for(u64 i = 0; i < mem_map->entries; i++) {
        kprintf("Section %d, base in pages: 0x%p, length: 0x%p, ", i, mem_map->memmap[i].base / 0x1000, mem_map->memmap[i].length);
        switch(mem_map->memmap[i].type) {
            case 1:
                kprintf("usable\n");
                avail_total += mem_map->memmap[i].length;
                break;
            case 2:
                kprintf("reserved\n");
                break;
            case 3:
                kprintf("acpi_reclaimable\n");
                break;
            case 4:
                kprintf("acpi_nvs\n");
                break;
            case 5:
                kprintf("bad memory\n");
                break;
            case 0x1000:
                kprintf("bootloader_reclaimable\n");
                break;
            case 0x1001:
                kprintf("kernel/modules\n");
                break;
            case 0x1002:
                kprintf("framebuffer\n");
                break;
        }
    }
    kprintf("usable mem: %d pages, %u bytes, %d mb\n", avail_total / 0x1000, avail_total / 1024, avail_total / 1024 / 1024);
    
    kprintf("Hello %s\n", "World!");
    init_descriptor_tables();
    init_interrupt_handlers();
    
    asm volatile ("int $0x3"); 

    for(;;) {
        asm("hlt");
    }

}



