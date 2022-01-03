#pragma once

#include <stdint.h>

typedef struct {
	uint8_t present : 1;
	uint8_t rw : 1;
	uint8_t user : 1;
	uint8_t write_through : 1;
	uint8_t cache_disable : 1;
	uint8_t accessed : 1;
	uint8_t ignored : 1;
	uint8_t zero : 2;
	uint8_t available : 3;
	uint64_t addr : 40;
	uint16_t available2 : 11;
	uint8_t no_execute : 1;
} page_l4_entry_t;

typedef struct {
	uint8_t present : 1;
	uint8_t rw : 1;
	uint8_t user : 1;
	uint8_t write_through : 1;
	uint8_t cache_disable : 1;
	uint8_t accessed : 1;
	uint8_t ignored : 1;
	uint8_t page_size : 1;
	uint8_t ignored1 : 1;
	uint8_t available : 3;
	uint64_t addr : 40;
	uint16_t available2 : 11;
	uint8_t no_execute : 1;
} page_directory_ptr_entry_t;


typedef struct {
	uint8_t present : 1;
	uint8_t rw : 1;
	uint8_t user : 1;
	uint8_t write_through : 1;
	uint8_t cache_disable : 1;
	uint8_t accessed : 1;
	uint8_t ignored : 1;
	uint8_t page_size : 1;
	uint8_t ignored1 : 1;
	uint8_t available : 3;
	uint64_t addr : 40;
	uint16_t available2 : 11;
	uint8_t no_execute : 1;
} page_directory_entry_t;

typedef struct {
	uint8_t present : 1;
	uint8_t rw : 1;
	uint8_t user : 1;
	uint8_t write_through : 1;
	uint8_t cache_disable : 1;
	uint8_t accessed : 1;
	uint8_t dirty : 1;
	uint8_t page_size : 1;
	uint8_t global : 1;
	uint8_t available : 3;
	uint64_t addr : 40;
	uint16_t available2 : 11;
	uint8_t no_execute : 1;
} page_table_entry_t;

void vmm_init();
