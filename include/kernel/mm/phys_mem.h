#pragma once

#include "stivale2.h"
#include <stddef.h>
#include "util/linked.h"

#define BUDDY_MAX_ORDER 10


typedef struct free_area {
    struct list_header free_list;
    uint64_t phys_base;
    size_t len;
} free_area_t;




