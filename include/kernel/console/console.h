#pragma once

#include "stivale2.h"
#include <stdarg.h>

void init_console(struct stivale2_struct_tag_framebuffer* fb_tag);

void putchar(char c);

void printf(const char* fmt, ...);
