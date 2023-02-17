#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "limine.h"

#include "acpi/rsdt.h"
#include "cpu/gdt.h"
#include "cpu/interrupts/idt.h"
#include "fbcon.h"
#include "libk/kutil.h"
#include "log.h"
#include "mm/init.h"

static volatile struct limine_rsdp_request rsdp_rqst = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0,
};

static volatile struct limine_framebuffer_request fb_rqst = {
    .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};

static volatile struct limine_memmap_request mm_rqst = {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0};

static volatile struct limine_kernel_address_request kaddr_rqst = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST, .revision = 0};

static void done(void) {
  for (;;) {
    __asm__("hlt");
  }
}

void _start(void) {
  if (fb_rqst.response == NULL || fb_rqst.response->framebuffer_count < 1) {
    done();
  }

  initialize_fbcon(fb_rqst.response);
  gdt_assemble();
  init_idt();

  mm_early_init(mm_rqst.response);
  for (int i = 0; i < 5; i++) {
    dbg("main :: page: 0x%p\n", mm_alloc_page());
  }
  void *ptr = (void *)0x41000;
  for (int i = 0; i < 5; i++) {
    mm_free_page(ptr);
    ptr += 0x1000;
  }
  for (int i = 0; i < 20; i++) {
    dbg("main :: page: 0x%p\n", mm_alloc_page());
  }
  dbg("acpi :: RSDP: %p\n", rsdp_rqst.response->address);
  acpi_rsdp_v2_t *rsdp = (void *)(rsdp_rqst.response->address);
  kprintf("ACPI rev: %d\n", rsdp->revision);
  if (acpi_rsdp_checksum(rsdp)) {
    dbg("acpi :: RSDP OK\n");
    acpi_header_t *madt = acpi_find_table(rsdp, "APIC");
    dbg("acpi :: MADT: 0x%p", madt);
  }
  done();
}
