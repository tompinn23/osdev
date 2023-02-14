#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

void memset(uint8_t *dest, uint8_t val, uint32_t len);
void memzero(void *ptr, uint32_t len);

int strlen(const char *s);
int strcpy(char *dst, const char *src);
int strncmp(const char *a, const char *b, int len);

void itoa(char *buf, unsigned long int n, int base);
int atoi(char *string);

int isspace(char c);
int isprint(char c);
