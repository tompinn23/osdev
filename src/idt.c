#include "idt.h"

extern void idt_flush(u32);

static void init_idt();
static void init_idt_desc(u32 offset, u16 select, u8 flags, idt_entry_t* desc);

void init_idt_desc(u32 offset, u16 select, u8 flags, idt_entry_t* desc) {
    desc->base_lo = offset & 0xFFFF;
    desc->base_hi = (offset >> 16) & 0xFFFF;
    desc->sel = select;
    desc->zero = 0;
    desc->flags = flags;
    
}

void init_idt() {

}

