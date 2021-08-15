#include "util.h"

#include "console/console.h"

void panic(const char* msg) {
    puts("PANIC: ");
    puts(msg);
    for (;;);
}