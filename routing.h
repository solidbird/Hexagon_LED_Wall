#ifndef ROUTING_H
#define ROUTING_H

#include <raylib.h>

typedef struct RGB_Value {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} RGB_Value;

typedef struct Frame {
	uint64_t route;
	RGB_Value rgb_value [127];
} Frame;

#endif
