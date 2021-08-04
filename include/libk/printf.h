#pragma once

#include <stdarg.h>

#include "int_types.h"

int is_format_letter(char c);

void vsprintf(char* str, void (*putchar)(char), const char* format, va_list arg);

void vsprintf_helper(char* str, void (*putchar)(char), const char* format, u32* pos, va_list arg);

void printf(const char* s, ...);

void serial_printf(const char* s, ...);
