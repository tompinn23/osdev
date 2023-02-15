#include "idt.h"

#include "cpu/gdt.h"

#include <stdbool.h>

static idtr_t idtr;
__attribute__((aligned(0x10))) static idt_entry_t idt[IDT_MAX_DESCRIPTORS];
static bool vectors[IDT_MAX_DESCRIPTORS];

void exception_handler() {
  __asm__ volatile("cli");
  for (;;) {
    __asm__ volatile("hlt");
  }
}

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags) {
  idt_entry_t *desc = &idt[vector];
  desc->isr_low = (uint64_t)isr & 0xFFFF;
  desc->kernel_cs = GDT_OFFSET_KERNEL_CODE;
  desc->ist = 0;
  desc->attributes = flags;
  desc->isr_mid = ((uint64_t)isr >> 16) & 0xFFFF;
  desc->isr_high = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
  desc->reserved = 0;
}

extern void *isr_stub_table[];

void init_idt() {
  idtr.base = (uintptr_t)&idt[0];
  idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;

  for (uint8_t v = 0; v < 32; v++) {
    idt_set_descriptor(v, isr_stub_table[v], 0x8E);
    vectors[v] = true;
  }
  idt_reload(&idtr);
}
