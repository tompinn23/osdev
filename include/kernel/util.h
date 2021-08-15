#pragma once

#define DIV_ROUND_UP(val, div) (((val) + (div) - 1) / (div))


#define MIN(x, y) ({ \
	const typeof((x)) _x = (x); \
	const typeof((y)) _y = (y); \
	_x > _y ? _y : _x; })

#define MAX(x, y) ({ \
	const typeof((x)) _x = (x); \
	const typeof((y)) _y = (y); \
	_x > _y ? _x : _y; })

#define clamp(x, upper, lower) (MIN(upper, MAX(x, lower)))

void panic(const char* msg);