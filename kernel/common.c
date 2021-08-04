#include "common.h"



void outb(u16 port, u8 val) {
    asm volatile("outb %1, %0" : : "dN"(port), "a"(val));
}

u8 inb(u16 port) {
    u8 ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
