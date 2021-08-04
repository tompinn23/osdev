#include "terminal.h"

#include "int_types.h"
#include "string.h"

static struct terminal_fn {
    void (*term_write)(const char* str, size_t length);
} terminal;

void terminal_init(struct stivale2_struct_tag_terminal* term_tag) {
    terminal.term_write = term_tag->term_write;
}

void terminal_write(const char* str) {
    terminal.term_write(str, strlen(str));
}

void terminal_putchar(char c) {
    terminal.term_write(&c, 1);
}

void terminal_write_int(int num) {
    char buf[32];
    char buf2[32];
    int quotient = num / 10;
    int remainder = num % 10;
    int i = 0;
    for(;;) {
       buf[i] = remainder + 0x30;
       if(quotient == 0 || i == 31) {
            break;
       } else {
            num = quotient;
            quotient = num / 10;
            remainder = num % 10;
       }
       i++;
    }
    for(int j = 0; j < i + 1; j++) {
        buf2[j] = buf[i - j];
    }
    terminal.term_write(buf2, i);
}

