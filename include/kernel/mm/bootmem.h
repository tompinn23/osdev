#pragma once

struct bootmen_data {
    uint64_t *bitmap;
};

void* bootmem_alloc()