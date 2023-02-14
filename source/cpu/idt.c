#include "idt.h"

#include "libk/kutil.h"

#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

extern void idt_flush(uint64_t);
extern void gdt_flush(uint64_t);

gdt_entry_t gdt_entries[5];
gdt_ptr_t gdt_ptr;

idt_entry_t idt_entries[256];
idt_ptr_t idt_ptr;

static void init_gdt();
static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit,
                         uint32_t access, uint8_t gran);
static void gdt_reload_selectors(uint16_t code, uint16_t data);

static void init_idt();
static void init_idt_desc(int32_t num, uint64_t offset, uint16_t select,
                          uint8_t flags);

void init_descriptor_tables() {
  init_gdt();
  init_idt();
}

void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint32_t access,
                  uint8_t gran) {
  gdt_entries[num].base_low = (base & 0xFFFF);
  gdt_entries[num].base_miidle = (base >> 16) & 0xFF;
  gdt_entries[num].base_high = (base >> 24) & 0xFF;

  gdt_entries[num].limit_low = (limit & 0xFFFF);
  gdt_entries[num].granularity = gran;
  gdt_entries[num].access = access;
}

void init_gdt() {
  gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
  gdt_ptr.base = (uint64_t)&gdt_entries;

  gdt_set_gate(0, 0, 0, 0, 0); // NULL entries
  gdt_set_gate(1, 0, 0xFFFFF, 0x9A, 0xA);
  gdt_set_gate(2, 0, 0xFFFFF, 0x92, 0xC);
  gdt_set_gate(3, 0, 0xFFFFF, 0xFA, 0xA);
  gdt_set_gate(4, 0, 0xFFFFF, 0xF2, 0xC);

  gdt_flush((uint64_t)&gdt_ptr);
  //    __asm__ volatile("lgdt %0" :: "m"(gdt_ptr) : "memory");
  //    gdt_reload_selectors(0x08, 0x10);
  // kprintf("GDT initialized\n");
}

void gdt_reload_selectors(uint16_t code, uint16_t data) {}

void init_idt_desc(int32_t num, uint64_t offset, uint16_t select,
                   uint8_t flags) {
  idt_entries[num].base_lo = (uint16_t)offset;
  idt_entries[num].base_mid = (uint16_t)(offset >> 16);
  idt_entries[num].base_hi = (uint32_t)(offset >> 32);
  idt_entries[num].sel = select;
  idt_entries[num].ist = 0;
  idt_entries[num].zero = 0;
  idt_entries[num].type_attr = flags;
}

void init_idt() {
  idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
  idt_ptr.base = (uint64_t)&idt_entries;

  memzero(&idt_entries, sizeof(idt_entry_t) * 256);

  init_idt_desc(0, (uint64_t)isr0, 0x08, 0x8E);
  init_idt_desc(1, (uint64_t)isr1, 0x08, 0x8E);
  init_idt_desc(2, (uint64_t)isr2, 0x08, 0x8E);
  init_idt_desc(3, (uint64_t)isr3, 0x08, 0x8E);
  init_idt_desc(4, (uint64_t)isr4, 0x08, 0x8E);
  init_idt_desc(5, (uint64_t)isr5, 0x08, 0x8E);
  init_idt_desc(6, (uint64_t)isr6, 0x08, 0x8E);
  init_idt_desc(7, (uint64_t)isr7, 0x08, 0x8E);
  init_idt_desc(8, (uint64_t)isr8, 0x08, 0x8E);
  init_idt_desc(9, (uint64_t)isr9, 0x08, 0x8E);
  init_idt_desc(10, (uint64_t)isr10, 0x08, 0x8E);
  init_idt_desc(11, (uint64_t)isr11, 0x08, 0x8E);
  init_idt_desc(12, (uint64_t)isr12, 0x08, 0x8E);
  init_idt_desc(13, (uint64_t)isr13, 0x08, 0x8E);
  init_idt_desc(14, (uint64_t)isr14, 0x08, 0x8E);
  init_idt_desc(15, (uint64_t)isr15, 0x08, 0x8E);
  init_idt_desc(16, (uint64_t)isr16, 0x08, 0x8E);
  init_idt_desc(17, (uint64_t)isr17, 0x08, 0x8E);
  init_idt_desc(18, (uint64_t)isr18, 0x08, 0x8E);
  init_idt_desc(19, (uint64_t)isr19, 0x08, 0x8E);
  init_idt_desc(20, (uint64_t)isr20, 0x08, 0x8E);
  init_idt_desc(21, (uint64_t)isr21, 0x08, 0x8E);
  init_idt_desc(22, (uint64_t)isr22, 0x08, 0x8E);
  init_idt_desc(23, (uint64_t)isr23, 0x08, 0x8E);
  init_idt_desc(24, (uint64_t)isr24, 0x08, 0x8E);
  init_idt_desc(25, (uint64_t)isr25, 0x08, 0x8E);
  init_idt_desc(26, (uint64_t)isr26, 0x08, 0x8E);
  init_idt_desc(27, (uint64_t)isr27, 0x08, 0x8E);
  init_idt_desc(28, (uint64_t)isr28, 0x08, 0x8E);
  init_idt_desc(29, (uint64_t)isr29, 0x08, 0x8E);
  init_idt_desc(30, (uint64_t)isr30, 0x08, 0x8E);
  init_idt_desc(31, (uint64_t)isr31, 0x08, 0x8E);

  __asm__ volatile("lidt %0" ::"m"(idt_ptr));
  // kprintf("IDT initialized\n");
}
