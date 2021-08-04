#pragma  once

#include <stddef.h>

#include "stivale2.h"

#define ALIGNUP(val, align) ({ \
	const typeof((val)) _val = (val); \
	const typeof((align)) _align = (align); \
	(_val + (_align - 1)) & -_align; })

#define ALIGNDOWN(val, align) ({ \
	const typeof((val)) _val = (val); \
	const typeof((align)) _align = (align); \
	_val & ~(_align - 1); })

void pmm_init(struct stivale2_struct_tag_memmap* memory_map);


void* kmalloc(size_t size);
void kfree(void* ptr);
