#pragma once

#include <stdint.h>

struct rsdp_desc {
	char sig[8];
	uint8_t checksum;
	char oem_id[6];
	uint8_t revision;
	uint32_t address;
} __attribute__ ((packed));

struct rsdp2_desc {
	struct rsdp_desc first;

	uint32_t length;
	uint64_t xsdt_addr;
	uint8_t ex_checksum;
	uint8_t reserved[3];
} __attribute__ ((packed));

struct sdt_header {
	char sig[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oem_id[6];
	char oem_table_id[8];
	uint32_t oem_rev;
	uint32_t creator_id;
	uint32_t creator_rev;
};

struct xsdt {
	struct sdt_header h;
	struct sdt_header* sdt_headers[];
} __attribute__ ((packed));


int validate_sdt(struct sdt_header* header);

