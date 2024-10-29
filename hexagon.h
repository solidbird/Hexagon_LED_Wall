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
    float centerX;  // X coordinate of the hexagon center
    float centerY;  // Y coordinate of the hexagon center
    float radius;   // Radius of the hexagon
	Color color;
} Hexagon;

typedef struct HexagonPanel {
	float centerX;
	float centerY;
	float radius;
	int hexagonCount;
	struct HexagonPanel *peer_out[3];
	struct HexagonPanel *peer_in[3];
	uint8_t buffer_out[3][1024];
	uint8_t buffer_in[3][1024];
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

void drawHexagon(Hexagon hex, int index);
Hexagon* generateHexagons(Vector2 center, int* hexagonCount);
void* polling_buffers(void *arg);

//typedef struct Polling_args {
	
//} Polling_args;


#endif
