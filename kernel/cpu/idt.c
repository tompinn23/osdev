#include "cpu/idt.h"

#include "string.h"
#include "console/console.h"

#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

extern void idt_flush(u64);
extern void gdt_flush(u64);


gdt_entry_t gdt_entries[5];
gdt_ptr_t gdt_ptr;

idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

static void init_gdt();
static void gdt_set_gate(s32 num, u32 base, u32 limit, u32 access, u8 gran);
static void gdt_reload_selectors(u16 code, u16 data);

static void init_idt();
static void init_idt_desc(s32 num, u64 offset, u16 select, u8 flags);

void init_descriptor_tables() {
    init_gdt();
    init_idt();
}


void gdt_set_gate(s32 num, u32 base, u32 limit, u32 access, u8 gran) {
    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_miidle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low = (limit & 0xFFFF);
    gdt_entries[num].granularity = gran;
    gdt_entries[num].access = access;
}

void init_gdt() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdt_ptr.base = (u64)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0); // NULL entries
    gdt_set_gate(1, 0, 0, 0x9A, 0x20);
    gdt_set_gate(2, 0, 0, 0x92, 0);
    gdt_set_gate(3, 0, 0, 0xFA, 0x20);
    gdt_set_gate(4, 0, 0, 0xF2, 0);

    gdt_flush((u64)&gdt_ptr);
//    __asm__ volatile("lgdt %0" :: "m"(gdt_ptr) : "memory");
//    gdt_reload_selectors(0x08, 0x10);
    kprintf("GDT initialized\n");
}

void gdt_reload_selectors(uint16_t code, uint16_t data) {
}


void init_idt_desc(s32 num, u64 offset, u16 select, u8 flags) {
    idt_entries[num].base_lo = (u16)offset;
    idt_entries[num].base_mid = (u16)(offset >> 16);
    idt_entries[num].base_hi = (u32)(offset >> 32);
    idt_entries[num].sel = select;
    idt_entries[num].ist = 0;
    idt_entries[num].zero = 0;
    idt_entries[num].type_attr = flags;
}


void init_idt() {
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base = (u64)&idt_entries;
    
    memzero(&idt_entries, sizeof(idt_entry_t) * 256);

    init_idt_desc(0, (u64)isr0, 0x08, 0x8E);
    init_idt_desc(1, (u64)isr1, 0x08, 0x8E);
    init_idt_desc(2, (u64)isr2, 0x08, 0x8E);
    init_idt_desc(3, (u64)isr3, 0x08, 0x8E);
    init_idt_desc(4, (u64)isr4, 0x08, 0x8E);
    init_idt_desc(5, (u64)isr5, 0x08, 0x8E);
    init_idt_desc(6, (u64)isr6, 0x08, 0x8E);
    init_idt_desc(7, (u64)isr7, 0x08, 0x8E);
    init_idt_desc(8, (u64)isr8, 0x08, 0x8E);
    init_idt_desc(9, (u64)isr9, 0x08, 0x8E);
    init_idt_desc(10, (u64)isr10, 0x08, 0x8E);
    init_idt_desc(11, (u64)isr11, 0x08, 0x8E);
    init_idt_desc(12, (u64)isr12, 0x08, 0x8E);
    init_idt_desc(13, (u64)isr13, 0x08, 0x8E);
    init_idt_desc(14, (u64)isr14, 0x08, 0x8E);
    init_idt_desc(15, (u64)isr15, 0x08, 0x8E);
    init_idt_desc(16, (u64)isr16, 0x08, 0x8E);
    init_idt_desc(17, (u64)isr17, 0x08, 0x8E);
    init_idt_desc(18, (u64)isr18, 0x08, 0x8E);
    init_idt_desc(19, (u64)isr19, 0x08, 0x8E);
    init_idt_desc(20, (u64)isr20, 0x08, 0x8E);
    init_idt_desc(21, (u64)isr21, 0x08, 0x8E);
    init_idt_desc(22, (u64)isr22, 0x08, 0x8E);
    init_idt_desc(23, (u64)isr23, 0x08, 0x8E);
    init_idt_desc(24, (u64)isr24, 0x08, 0x8E);
    init_idt_desc(25, (u64)isr25, 0x08, 0x8E);
    init_idt_desc(26, (u64)isr26, 0x08, 0x8E);
    init_idt_desc(27, (u64)isr27, 0x08, 0x8E);
    init_idt_desc(28, (u64)isr28, 0x08, 0x8E);
    init_idt_desc(29, (u64)isr29, 0x08, 0x8E);
    init_idt_desc(30, (u64)isr30, 0x08, 0x8E);
    init_idt_desc(31, (u64)isr31, 0x08, 0x8E);
    
    __asm__ volatile("lidt %0" :: "m"(idt_ptr));
    kprintf("IDT initialized\n");
}

