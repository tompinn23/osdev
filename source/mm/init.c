#include "init.h"

#include "buddy.h"
#include "libk/intrusive_list.h"
#include "log.h"
#define PAGE_SIZE 0x1000
#define HIGHMEM_LIMIT 0x100000000

static struct mm_info {
  // page_t* pages;
  uint64_t page_count;
  struct intrusive_list buddies;
  int early;
} mm_info;

void mm_early_init(struct limine_memmap_response *mm) {
  intrusive_list_init(&mm_info.buddies);
  mm_info.early = 1;
  for (int i = 0; i < mm->entry_count; i++) {
    switch (mm->entries[i]->type) {
    case LIMINE_MEMMAP_USABLE:
      dbg("mm_early :: usable :: base 0x%p :: size 0x%p\n",
          mm->entries[i]->base, mm->entries[i]->length);
      buddy_t *early =
          buddy_init(ZONE_LOWMEM, mm->entries[i]->base, mm->entries[i]->length);
      intrusive_list_push(&mm_info.buddies, &early->buddies);
      return;
    }
  }
}

void mm_init(struct limine_memmap_response *mm) {
  mm_info.early = 0;
  for (int i = 0; i < mm->entry_count; i++) {
    switch (mm->entries[i]->type) {
    case LIMINE_MEMMAP_USABLE:
      dbg("mm :: usable :: base 0x%p :: size 0x%p\n", mm->entries[i]->base,
          mm->entries[i]->length);
      mm_info.page_count += (mm->entries[i]->length / PAGE_SIZE);
      buddy_t *bud =
          buddy_init(ZONE_LOWMEM, mm->entries[i]->base, mm->entries[i]->length);
      intrusive_list_push(&mm_info.buddies, &bud->buddies);
      break;
    case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE:
      dbg("mm :: bt_reclaimable :: base 0x%p :: size 0x%p\n",
          mm->entries[i]->base, mm->entries[i]->length);
      mm_info.page_count += (mm->entries[i]->length / PAGE_SIZE);
      break;
    }
  }
  dbg("mm :: page count %d\n", mm_info.page_count);
}

void *mm_alloc_page() {
  if (mm_info.early) {
    buddy_t *b = intrusive_list_container_of(mm_info.buddies.next, b, buddies);
    return buddy_alloc_page(b);
  }
  return NULL;
}
