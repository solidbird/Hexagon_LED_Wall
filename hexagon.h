#ifndef HEXAGON_H
#define HEXAGON_H

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <raylib.h>
#include "sts_queue/sts_queue.h"

#define largeHexRadius 120.0  // Radius of the large hexagon
#define smallHexRadius 20.0   // Radius of the smaller hexagons

typedef struct {
	int r;
	int q;
} Coordinates;

// Structure to represent a hexagon
typedef struct {
	Vector2 center;
    float radius;   // Radius of the hexagon
	Coordinates coordinates;
	Color color;
} Hexagon;

typedef struct Buffer {
	//uint8_t data[1024];
	pthread_t thread;
	StsQueue data;
} Buffer;

typedef struct HexagonPanel {
	Vector2 center;
	float radius;
	int index;
	int hexagonCount;
	struct HexagonPanel *peer_out[3];
	struct HexagonPanel *peer_in[3];
	Buffer buffer_out[3];
	Buffer buffer_in[3];
	Hexagon *pixels;
} HexagonPanel;

typedef struct {
	HexagonPanel* hexagon_panel;
	int buffer_in_index;
	int buffer_out_index;
} Polling_args;

extern const Vector2 dock_top_right;
extern const Vector2 dock_right;
extern const Vector2 dock_bottom_right;
extern const Vector2 dock_bottom_left;
extern const Vector2 dock_left;
extern const Vector2 dock_top_left;

void drawHexagon(Hexagon* hex, int index);
Hexagon* generateHexagons(Vector2 center, int* hexagonCount);
void process_stuff(HexagonPanel* hp, int index);
void clear_screen(HexagonPanel* hp);
void* polling_buffers(void *arg);

#endif
