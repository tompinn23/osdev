#pragma once

#include <stdarg.h>
#include <stdint.h>

void vsprintf_helper(char *buf, void (*putchar)(char), const char *format,
                     uint32_t *pos, va_list arg);
