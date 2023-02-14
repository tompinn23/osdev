#pragma once

#ifndef _NDEBUG
#define dbg(...) kdbg_early(__FILE__, __LINE__, __VA_ARGS__)
#else
#define dbg(...)
#endif

void kdbg_early(const char *file, int line, const char *fmt, ...);
