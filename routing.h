#ifndef ROUTING_H
#define ROUTING_H

#include <raylib.h>

typedef struct Payload {
	uint8_t panel_id;
	uint8_t hops;
	Color pixel_color;
	uint8_t pos;
} Payload;

typedef struct Routing_chain {
	uint8_t panel_id;
	uint8_t gateway;
	uint8_t children[50];
} Routing_chain;

/*typedef struct Frame {
	uint8_t magic_number;
	Payload payload;
} Frame;*/

#endif
