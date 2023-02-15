#include "rsdt.h"

#include "libk/kutil.h"
#include <stdbool.h>
#include <stddef.h>

int acpi_rsdp_checksum(acpi_rsdp_v2_t *rsdp) {
  unsigned int sum = 0;
  for (int i = 0; i < offsetof(acpi_rsdp_v2_t, length); i++) {
    sum += ((char *)rsdp)[i];
  }
  if (rsdp->revision == 0) {
    return (sum & 0xFF) == 0;
  }
  if ((sum & 0xFF) != 0) {
    return 0;
  }
  sum = 0;
  for (unsigned long i = 0; i < sizeof(*rsdp) - 3; i++) {
    sum += ((char *)rsdp)[i];
  }
  return (sum & 0xFF) == 0;
}

static acpi_header_t *acpi_rsdt_get(acpi_rsdp_v2_t *rsdp, uint16_t index) {
  acpi_header_t *rsdt = (acpi_header_t *)((uint64_t)(rsdp->rsdt));
  uint64_t rsdt_entries =
      (rsdt->length - sizeof(acpi_header_t)) / sizeof(uintptr_t);
  if (index >= rsdt_entries)
    return NULL;
  uint32_t *rsdt_tbl =
      (uint32_t *)((uint64_t)rsdp->rsdt + sizeof(acpi_header_t));
  return (acpi_header_t *)((uint64_t)rsdt_tbl[index]);
}
static acpi_header_t *acpi_xsdt_get(acpi_rsdp_v2_t *rsdp, uint16_t index) {
  acpi_header_t *xsdt = (acpi_header_t *)((uint64_t)(rsdp->xsdt));
  uint64_t xsdt_entries =
      (xsdt->length - sizeof(acpi_header_t)) / sizeof(uintptr_t);
  if (index >= xsdt_entries)
    return NULL;
  uint64_t *xsdt_tbl =
      (uint64_t *)((uint64_t)rsdp->xsdt + sizeof(acpi_header_t));
  return (acpi_header_t *)((uint64_t)xsdt_tbl[index]);
}

acpi_header_t *acpi_find_table(acpi_rsdp_v2_t *rsdp, const char *key) {
  if (!rsdp)
    return NULL;
  bool xsdt = (!!rsdp->xsdt);
  for (uint16_t i = 0;; i++) {
    acpi_header_t *header;
    if (xsdt) {
      header = acpi_xsdt_get(rsdp, i);
    } else {
      header = acpi_rsdt_get(rsdp, i);
    }
    if (!header)
      break;
    if (!strncmp((const char *)&header->sig[0], key, 4))
      return header;
  }
  return NULL;
}
