#include "rsdt.h"

#include "libk/kutil.h"
#include <stddef.h>

int rsdp_checksum(rsdp_descriptor_v1_t *rsdp) {
  unsigned int sum = 0;
  for (int i = 0; i < sizeof(*rsdp); i++) {
    sum += ((char *)rsdp)[i];
  }
  return (sum & 0xFF) == 0;
}

int rsdp2_checksum(rsdp_descriptor_v2_t *rsdp) {
  if (rsdp_checksum((rsdp_descriptor_v1_t *)rsdp) == 0) {
    return 0;
  }
  unsigned int sum = 0;
  for (unsigned long i = 0; i < sizeof(*rsdp) - 3; i++) {
    sum += ((char *)rsdp)[i];
  }
  return (sum & 0xFF) == 0;
}

int rsdp_is_v2(void *ptr) {
  rsdp_descriptor_v1_t *rsdp = ptr;
  return rsdp->revision != 0;
}

acpi_header_t *acpi_v1_find_table(acpi_header_t *rsdt, const char *key) {
  rsdt_t *tbl = rsdt;
  int entries = (tbl->header.length - sizeof(tbl->header)) / 4;
  for (int i = 0; i < entries; i++) {
    acpi_header_t *t = (acpi_header_t *)tbl->SDTs[i];
    if (!strncmp(t->sig, key, 4)) {
      return t;
    }
  }
  return NULL;
}
