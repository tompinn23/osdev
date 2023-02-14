#include "kutil.h"

int strlen(const char *s) {
  int len = 0;
  while (*s++)
    len++;
  return len;
}

int strcpy(char *dst, const char *src) {
  int i = 0;
  while ((*dst++ = *src++) != 0)
    i++;
  return i;
}

int strncmp(const char *a, const char *b, int len) {
  for (int i = 0; i < len; i++) {
    if (*a < *b) {
      return -1;
    } else if (*a > *b) {
      return 1;
    }
  }
  return 0;
}

void itoa(char *buf, unsigned long int n, int base) {
  unsigned long int tmp;
  int i, j;

  tmp = n;
  i = 0;

  do {
    tmp = n % base;
    buf[i++] = (tmp < 10) ? (tmp + '0') : (tmp + 'a' - 10);
  } while (n /= base);
  buf[i--] = 0;

  for (j = 0; j < i; j++, i--) {
    tmp = buf[j];
    buf[j] = buf[i];
    buf[i] = tmp;
  }
}

int atoi(char *string) {
  int result = 0;
  unsigned int digit;
  int sign;

  while (isspace(*string)) {
    string += 1;
  }

  /*
   * Check for a sign.
   */

  if (*string == '-') {
    sign = 1;
    string += 1;
  } else {
    sign = 0;
    if (*string == '+') {
      string += 1;
    }
  }

  for (;; string += 1) {
    digit = *string - '0';
    if (digit > 9) {
      break;
    }
    result = (10 * result) + digit;
  }

  if (sign) {
    return -result;
  }
  return result;
}

int isspace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' ||
         c == '\r';
}

int isprint(char c) { return ((c >= ' ' && c <= '~') ? 1 : 0); }

void memset(uint8_t *dest, uint8_t val, uint32_t len) {
  uint8_t *temp = (uint8_t *)dest;
  for (; len != 0; len--)
    *temp++ = val;
}

void memzero(void *ptr, uint32_t len) { memset(ptr, 0, len); }
