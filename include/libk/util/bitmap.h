#pragma once

typedef struct bitmap {
    uint64_t* map_data;
    uint32_t len;
} bitmap_t;
