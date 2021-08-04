#include "string.h"

int strlen(const char* s) {
    int len = 0;
    while(*s++)
        len++;
    return len;
}

int strcpy(char *dst,const char *src) {
    int i = 0;
    while ((*dst++ = *src++) != 0)
        i++;
    return i;
}

void itoa(char *buf, unsigned long int n, int base)
{
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

int atoi(char * string) {
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

    for ( ; ; string += 1) {
        digit = *string - '0';
        if (digit > 9) {
            break;
        }
        result = (10*result) + digit;
    }

    if (sign) {
        return -result;
    }
    return result;
}

int isspace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

int isprint(char c) {
    return ((c >= ' ' && c <= '~') ? 1 : 0);
}

void memset(u8* dest, u8 val, u32 len) {
    u8* temp = (u8*)dest;
    for( ; len != 0; len--) *temp++ = val;
}

void memzero(void* ptr, u32 len) {
    memset(ptr, 0, len);
}