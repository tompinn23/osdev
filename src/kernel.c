#include <stdint.h>
#include <stddef.h>

#include "stivale2.h"
#include "console/console.h"
#include "klib/printf.h"
#include "cpu/idt.h"
#include "kernel/isr.h"

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

void _start(struct stivale2_struct* stivale2_struct) {
    struct stivale2_struct_tag_framebuffer* term_str_tag;
    term_str_tag = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);
    if(term_str_tag == NULL) {
        for(;;) {
            asm ("hlt");
        }
    }
    
    init_console(term_str_tag);

    printf("Hello %s\n", "World!");
    init_descriptor_tables();
    init_interrupt_handlers();
    
    asm volatile ("int $0x3"); 

    for(;;) {
        asm("hlt");
    }

}



