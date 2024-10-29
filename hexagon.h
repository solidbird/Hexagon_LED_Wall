#ifndef HEXAGON_H
#define HEXAGON_H

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <raylib.h>

#define largeHexRadius 60.0  // Radius of the large hexagon
#define smallHexRadius 10.0   // Radius of the smaller hexagons

// Structure to represent a hexagon
typedef struct {
	Vector2 center;
    float radius;   // Radius of the hexagon
	Color color;
} Hexagon;

typedef struct {
	//uint8_t data[1024];
	int data[1024];
	int top;
} Buffer;

typedef struct HexagonPanel {
	Vector2 center;
	float radius;
	int hexagonCount;
	struct HexagonPanel *peer_out[3];
	struct HexagonPanel *peer_in[3];
	Buffer buffer_out[3];
	Buffer buffer_in[3];
	Hexagon *pixels;
	pthread_t thread;
} HexagonPanel;

typedef struct {
	HexagonPanel* hexagon_panel;
	int index;
} Polling_args;

extern const Vector2 dock_top_right;
extern const Vector2 dock_right;
extern const Vector2 dock_bottom_right;
extern const Vector2 dock_bottom_left;
extern const Vector2 dock_left;
extern const Vector2 dock_top_left;

int init_buffer(Buffer *buffer);
int push(Buffer *buffer, char value);
int pop(Buffer *buffer);
int is_empty(Buffer *buffer);
int is_full(Buffer *buffer);

void drawHexagon(Hexagon hex, int index);
Hexagon* generateHexagons(Vector2 center, int* hexagonCount);
void process_stuff(HexagonPanel* hp, int index);
void* polling_buffers(void *arg);

#endif
