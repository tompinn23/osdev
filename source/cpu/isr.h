#pragma once

#include <stdint.h>

typedef struct registers {
  uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rbp, rcx, rbx;
  uint64_t rdx, rsi, rdi;
  uint64_t es, ds, fs, rax;
  uint64_t ecode, type;
  uint64_t rip, cs, rflags, uesp, ss;
} registers_t;

typedef void (*isr_handler_t)(registers_t);

void init_interrupt_handlers();
