#include "common.h"

int sdt_checksum(acpi_header_t *table) {
  unsigned char sum = 0;
  for (int i = 0; i < table->length; i++) {
    sum += ((char *)table)[i];
  }
  return sum == 0;
}