#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "limine.h"

#include "acpi/rsdt.h"
#include "cpu/idt.h"
#include "fbcon.h"
#include "libk/kutil.h"
#include "log.h"

static volatile struct limine_rsdp_request rsdp_rqst = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0,
};

static volatile struct limine_framebuffer_request fb_rqst = {
    .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};

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

  dbg("acpi :: RSDP: %p\n", rsdp_rqst.response->address);
  rsdp_descriptor_v1_t *rsdp = rsdp_rqst.response->address;
  kprintf("ACPI rev: %d\n", rsdp->revision);
  done();
  if (rsdp_is_v2(rsdp)) {
    kprintf("ACPI ver: 2.0+\n");
  } else {
    kprintf("ACPI ver: 1.0\n");
    acpi_header_t *rsdt = (void *)rsdp->rsdt;
    acpi_header_t *madt = NULL;
    if (sdt_checksum(rsdt)) {
      madt = acpi_v1_find_table(rsdt, "APIC");
    }
  }
  done();
}
