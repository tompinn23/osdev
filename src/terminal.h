#pragma once

#include "stivale2.h"

void terminal_write(const char* str);
void terminal_putchar(char c);
void terminal_write_int(int num);


void terminal_init(struct stivale2_struct_tag_terminal* term_tag);
