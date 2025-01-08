#ifndef ROUTING_H
#define ROUTING_H

#include <raylib.h>
#include <stdint.h>

typedef struct RGB_Value {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} RGB_Value;

typedef struct Scene {
	Vector2 destination;
	RGB_Value rgb_value [127];
} Scene;

typedef struct Frame {
	uint64_t route;
	RGB_Value rgb_value [127];
} Frame;

typedef struct Discovery_package{
	uint64_t route_edges;
} Discovery_package;

#endif
