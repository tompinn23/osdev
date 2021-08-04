#include "isr.h"
#include "string.h"
#include "printf.h"



isr_handler_t isr_handlers[256];

void isr_handler(registers_t regs) {
    //    __asm__ volatile("xchgw %bx, %bx");
    if(isr_handlers[regs.type] != NULL) {
        isr_handlers[regs.type](regs);
        return;
    }
    printf("unhandled interrupt: %d", regs.type);
}

void set_isr_handler(s32 num, isr_handler_t func) {
    isr_handlers[num] = func;
}


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


void isr_div_error(registers_t regs) {
    printf("INT: DIV error\n");
}

void isr_debug_exception(registers_t regs) {
    printf("INT: Debug exception\n");
}

void isr_nmi_interrupt(registers_t regs) {
    printf("INT: NMI interrupt\n");
}

void isr_breakpoint(registers_t regs) {
    printf("INT: Breakpoint\n");
}


void isr_double_fault(registers_t regs) {
    printf("INT: Double Fault. Oh fuck!\n");
}

void isr_general_protection(registers_t regs) {
    printf("INT: General Protection fault");
}

void isr_page_fault(registers_t regs) {
    printf("INT: page fault :(\n");
}
