#include <stdint.h>
#include <stddef.h>

#include "stivale2.h"
#include "mm/memory.h"
#include "mm/bootmem.h"
#include "console/console.h"
#include "printf.h"
#include "cpu/idt.h"
#include "isr.h"

#include "acpi/init.h"

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
    .flags = (1 << 1) | (1 << 4),
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

    init_descriptor_tables();
    init_interrupt_handlers();

    init_console(term_str_tag); // init console  
	struct stivale2_struct_tag_rsdp* rsdp_tag;
	rsdp_tag = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_RSDP_ID);
//	parse_acpi_tables(rsdp_tag->rsdp);
	
	struct stivale2_struct_tag_kernel_base_address* addr_tag;
	addr_tag = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_KERNEL_BASE_ADDRESS_ID);
	if(addr_tag != NULL) {
		kprintf("kernel virt: 0x%p phys: 0x%p\n", addr_tag->virtual_base_address, addr_tag->physical_base_address);
	}
    kprintf("Kernel End: 0x%p\n", _kernel_end_phys);
/*
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
  */  
	bootmem_init(&page_data, mem_map);
    kprintf("Page data: %x\n", page_data);
	void* dat = bootmem_alloc(1);

    kprintf("Hello %s\n", "World!");
    
    asm volatile ("int $0x3"); 

    for(;;) {
        asm("hlt");
    }

}



