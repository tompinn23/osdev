#include "log.h"

#include "fbcon.h"

void kdbg_early(const char *file, int line, const char *fmt, ...) {
  kprintf("\x1B[34;2m[DBG]\x1B[0m %s:%d ", file, line);
  va_list ap;
  va_start(ap, fmt);
  vkprintf(fmt, ap);
  va_end(ap);
}
