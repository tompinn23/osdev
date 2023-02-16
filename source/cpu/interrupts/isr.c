#include "isr.h"

#include "fbcon.h"

#include <stddef.h>

#define PAGE_FAULT_PRESENT 0x1
#define PAGE_FAULT_WRITE 0x2
#define PAGE_FAULT_USER 0x4

void isr_exception_handler(isr_xframe_t *frame);

typedef void (*exception_handler)(isr_xframe_t *);

static void page_fault(isr_xframe_t *frame) {
  kprintf("ACCESS VIOLATION: %s\n",
          frame->base_frame.error_code & PAGE_FAULT_WRITE ? "WRITE" : "READ");
  kprintf("error: page fault: at 0x%p\n", frame->base_frame.rip);
  kprintf("pf attempted access: 0x%p\n", frame->control_registers.cr2);
  kprintf("pf err: 0x%p\n", frame->base_frame.error_code);
  for (;;) {
    __asm__ volatile("cli; hlt");
  }
}

static exception_handler handlers[32] = {
    [0] = NULL,  [1] = NULL,  [2] = NULL,  [3] = NULL,  [4] = NULL,
    [5] = NULL,  [6] = NULL,  [7] = NULL,  [8] = NULL,  [9] = NULL,
    [10] = NULL, [11] = NULL, [12] = NULL, [13] = NULL, [14] = page_fault,
    [15] = NULL, [16] = NULL, [17] = NULL, [18] = NULL, [19] = NULL,
    [20] = NULL, [21] = NULL, [22] = NULL, [23] = NULL, [24] = NULL,
    [25] = NULL, [26] = NULL, [27] = NULL, [28] = NULL, [29] = NULL,
    [30] = NULL, [31] = NULL,
};

void isr_exception_handler(isr_xframe_t *frame) {
  if (handlers[frame->base_frame.vector] != NULL) {
    handlers[frame->base_frame.vector](frame);
    return;
  }
  kprintf("error: cpu exception: 0x%p @ 0x%p\n", frame->base_frame.vector,
          frame->base_frame.rip);
  kprintf("error code: %d\n", frame->base_frame.error_code);
  for (;;) {
    __asm__ volatile("cli; hlt");
  }
}
