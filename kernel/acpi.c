#include "acpi/init.h"

#include "acpi/tables.h"

#include "printf.h"
#include "util.h"

#if defined(DEBUG_acpi) || defined(DEBUG_ALL)
#define dbg(args...) kprintf("acpi:: " args)
#else
#define dbg(fmt...)
#endif

static int rsdp_checksum(struct rsdp_desc* desc) {
	int chk = 0;
	for(size_t i = 0; i < sizeof(struct rsdp_desc); i++) {
		chk += ((uint8_t*)desc)[i];
	}
	return (chk & 0xFF) == 0;
}


static void parse_rsdp1(struct rsdp_desc* desc) {
	if(!rsdp_checksum(desc)){
		panic("failed to validate rsdp");
	}

	panic("implement 1.0 rsdp");
}

static void parse_rsdp2(struct rsdp2_desc* desc) {
	if(!rsdp_checksum(&(desc->first))){
		panic("failed to validate rsdp");
	}
	int chk = 0;
	for(size_t i = offsetof(struct rsdp2_desc, first); i < sizeof(struct rsdp2_desc); i++) {
		chk += ((uint8_t*)desc)[i];
	}
	if((chk & 0xFF) != 0) {
		panic("failed to validate ex rsdp");
	}

	if((void*)desc->xsdt_addr != NULL) {
		dbg("found xsdt addr: 0x%p\n", desc->xsdt_addr);
	}
	struct xsdt* xsdt = (void*)desc->xsdt_addr;
	if(!validate_sdt(&xsdt->h)) {
		panic("failed to validate xsdt");
	}
	int amt = (xsdt->h.length - sizeof(xsdt->h)) / sizeof(uint64_t);
	dbg("amt of tables: %d\n", amt);
	struct sdt_header* header;
	for(int i = 0; i < amt; i++) {
		header = xsdt->sdt_headers[i];
		dbg("table: %s\n", header->sig);
	}
}

void parse_acpi_tables(uint64_t rsdp) {
	kprintf("begin acpi parse\n");
	dbg("recieved rsdp: 0x%p\n", rsdp);
	struct rsdp2_desc* rsdp_header = (void*)rsdp;
	if(rsdp_header->first.revision == 0) {
		dbg("acpi ver: 1.0\n");
		parse_rsdp1((void*)rsdp_header);
	} else {
		dbg("acpi ver: 2.0+\n");
		parse_rsdp2(rsdp_header);
	}
}

int validate_sdt(struct sdt_header* header) {
	dbg("validating: 0x%p\n", header);
	unsigned char sum = 0;
	for(uint32_t i = 0; i < header->length; i++) {
		sum += ((char*)header)[i];
	}
	return sum == 0;
}
