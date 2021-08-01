#include "serial.h"

#include "common.h"

#define PORT 0x3f8

int init_serial() {
    outb(PORT + 1, 0x00);
    outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 1, 0x00);    //                  (hi byte)
    outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    outb(PORT + 4, 0x1E);    // Set in loopback mode, test the serial chip
    outb(PORT + 0, 0xAE);

    if(inb(PORT + 0) != 0xAE) {
        return 1;
    }

    outb(PORT + 4, 0x0F);
    return 0;
}

int serial_recieved() {
    return inb(PORT + 5) & 1;
}

char read_serial() {
    while(serial_recieved() == 0);

    return inb(PORT);
}

int is_transmit_empty() {
    return inb(PORT + 5) & 0x20;
}

void write_serial(char a) {
    while(is_transmit_empty() == 0);

    outb(PORT, a);
}
