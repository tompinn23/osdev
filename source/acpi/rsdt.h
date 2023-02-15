#pragma once

#include "common.h"
#include <stdint.h>

typedef struct rsdp_descriptor_v2 {
  uint8_t signature[8];
  uint8_t checksum;
  uint8_t vendor_id[6];
  uint8_t revision;
  uint32_t rsdt;
  uint32_t length;
  acpi_header_t *xsdt;
  uint8_t xchecksum;
  uint8_t rsv0[3];
} __attribute__((packed)) acpi_rsdp_v2_t;

int acpi_rsdp_checksum(acpi_rsdp_v2_t *rsdp);

acpi_header_t *acpi_find_table(acpi_rsdp_v2_t *rsdp, const char *key);
