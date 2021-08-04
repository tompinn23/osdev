#pragma  once

#include <stddef.h>

#include "stivale2.h"

void pmm_init(struct stivale2_struct_tag_memmap* mmap);

void* kmalloc(size_t size);
void kfree(void* ptr);
