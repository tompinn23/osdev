#include "isr.h"

#include "libk/kutil.h"

#include <stddef.h>

isr_handler_t isr_handlers[256];

void isr_handler(registers_t regs) {
  //    __asm__ volatile("xchgw %bx, %bx");
  if (isr_handlers[regs.type] != NULL) {
    isr_handlers[regs.type](regs);
    return;
  }
  // kprintf("unhandled interrupt: %d\n", regs.type);
}

void panic(const char *str) {
  for (;;) {
    __asm__("hlt");
  }
}

void set_isr_handler(int32_t num, isr_handler_t func) {
  isr_handlers[num] = func;
}

static void isr_div_error(registers_t regs) { panic("INT: DIV error\n"); }

static void isr_debug_exception(registers_t regs) {
  panic("INT: Debug exception\n");
}

static void isr_nmi_interrupt(registers_t regs) {
  panic("INT: NMI interrupt\n");
}

static void isr_breakpoint(registers_t regs) { panic("INT: Breakpoint\n"); }

static void isr_double_fault(registers_t regs) {
  panic("INT: Double Fault. Oh fuck!\n");
}

static void isr_general_protection(registers_t regs) {
  panic("INT: General Protection fault");
}

static void isr_page_fault(registers_t regs) { panic("INT: page fault :(\n"); }

void init_interrupt_handlers() {
  memzero(isr_handlers, sizeof(isr_handlers));
  set_isr_handler(0, isr_div_error);
  set_isr_handler(1, isr_debug_exception);
  set_isr_handler(2, isr_nmi_interrupt);
  set_isr_handler(3, isr_breakpoint);

  set_isr_handler(8, isr_double_fault);

  set_isr_handler(13, isr_general_protection);
  set_isr_handler(14, isr_page_fault);
}
