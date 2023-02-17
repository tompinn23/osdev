#pragma once

#include <limine.h>

void mm_init(struct limine_memmap_response *mm);
void mm_early_init(struct limine_memmap_response *mm);

void *mm_alloc_page();
void mm_free_page(void *page);
