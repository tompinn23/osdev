#pragma once

#include <limine.h>
#include <stdarg.h>

void initialize_fbcon(struct limine_framebuffer_response *rsp);

void kprintf(const char *fmt, ...);
void vkprintf(const char *fmt, va_list ap);
