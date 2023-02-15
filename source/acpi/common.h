#pragma once

#include <stdint.h>

typedef struct acpi_header {
  char sig[4];
  uint32_t length;
  uint8_t rev;
  uint8_t checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEM_rev;
  uint32_t creator_id;
  uint32_t creator_rev;
} __attribute__((packed)) acpi_header_t;

int sdt_checksum(acpi_header_t *table);
