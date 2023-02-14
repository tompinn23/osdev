#pragma once

#include "common.h"
#include <stdint.h>

typedef struct rsdp_descriptor_v1 {
  char sig[8];
  uint8_t checksum;
  char OEMID[6];
  uint8_t revision;
  uint32_t rsdt;
} __attribute__((packed)) rsdp_descriptor_v1_t;

typedef struct rsdp_descriptor_v2 {
  rsdp_descriptor_v1_t descriptor;
  uint32_t length;
  uint64_t xsdt;
  uint8_t extended_checksum;
  uint8_t reserved[3];
} __attribute__((packed)) rsdp_descriptor_v2_t;

typedef struct xsdt {
  struct acpi_header header;
  uint64_t *SDTs;
} xsdt_t;

typedef struct rsdt {
  struct acpi_header header;
  uint32_t *SDTs;
} rsdt_t;

int rsdp_checksum(rsdp_descriptor_v1_t *rsdp);
int rsdp2_checksum(rsdp_descriptor_v2_t *rsdp);

int rsdp_is_v2(void *ptr);

acpi_header_t *acpi_v1_find_table(acpi_header_t *hd, const char *key);
