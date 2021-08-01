#pragma once

#include "klib/int_types.h"

typedef struct registers{
    u64 r15, r14, r13, r12, r11, r10, r9, r8, rbp, rcx, rbx;
    u64 rdx, rsi, rdi;
    u64 es, ds, fs, rax;
    u64 ecode, type;
    u64 rip, cs, rflags, uesp, ss;
} registers_t ;

typedef void (*isr_handler_t)(registers_t);

void init_interrupt_handlers();

void isr_div_error(registers_t regs);
void isr_debug_exception(registers_t regs);
void isr_nmi_interrupt(registers_t regs);
void isr_breakpoint(registers_t regs);

void isr_double_fault(registers_t regs);
void isr_general_protection(registers_t regs);
void isr_page_fault(registers_t regs);
