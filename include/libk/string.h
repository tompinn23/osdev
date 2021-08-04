#pragma once

#include "int_types.h"

void memset(u8* dest, u8 val, u32 len);
void memzero(void* ptr, u32 len);

int strlen(const char* s);
int strcpy(char* dst, const char* src);

void itoa(char* buf, unsigned long int n, int base);
int atoi(char* string);

int isspace(char c);
int isprint(char c);
