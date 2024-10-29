
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "hexagon.h"

#define CLOCK_SPEED_A 80000000
#define CLOCK_SPEED_B 240000000

const Vector2 dock_top_right = {
	(largeHexRadius * 2 - smallHexRadius),
	-(largeHexRadius * 3 - smallHexRadius)
};
const Vector2 dock_right = {
	(largeHexRadius * 4 - smallHexRadius * 2),
	0
};
const Vector2 dock_bottom_right = {
	(largeHexRadius * 2 - smallHexRadius),
	(largeHexRadius * 3 - smallHexRadius)
};
const Vector2 dock_bottom_left = {
	-(largeHexRadius * 2 - smallHexRadius),
	(largeHexRadius * 3 - smallHexRadius)
};
const Vector2 dock_left = {
	-(largeHexRadius * 4 - smallHexRadius * 2),
	0
};
const Vector2 dock_top_left = {
	-(largeHexRadius * 2 - smallHexRadius),
	-(largeHexRadius * 3 - smallHexRadius)
};

void drawHexagon(Hexagon hex, int index){
	DrawPolyLines(
		(Vector2){hex.centerX, hex.centerY},
		6,
		hex.radius,
		0,
		WHITE	
	);

	DrawCircleV(
		(Vector2){hex.centerX, hex.centerY},
		hex.radius * 0.85,
		hex.color
	);

	char index_as_str[5];
	sprintf(index_as_str, "%d", index);	
	DrawText(index_as_str, hex.centerX - hex.radius/2, hex.centerY - hex.radius/2, 6, WHITE);
}

// Function to generate hexagons and store them in an array
Hexagon* generateHexagons(Vector2 center, int* hexagonCount) {
    int maxDistance = (int)(largeHexRadius / smallHexRadius);  // Number of hexagon layers
    int totalHexagons = 0;
	
	// First, calculate how many hexagons we will need
    for (int q = -maxDistance; q <= maxDistance; ++q) {
        int r1 = fmax(-maxDistance, -q - maxDistance);
        int r2 = fmin(maxDistance, -q + maxDistance);
        for (int r = r1; r <= r2; ++r) {
            totalHexagons++;
        }
    }
    // Allocate memory for the hexagons
    Hexagon* hexagons = (Hexagon*)malloc(totalHexagons * sizeof(Hexagon));

    // Now store each hexagon
    int index = 0;
    for (int q = -maxDistance; q <= maxDistance; ++q) {
        int r1 = fmax(-maxDistance, -q - maxDistance);
        int r2 = fmin(maxDistance, -q + maxDistance);
        for (int r = r1; r <= r2; ++r) {
            // Calculate the center of each small hexagon in Cartesian coordinates
            float centerX = smallHexRadius * (3.0 / 2.0 * q) + center.x;
            float centerY = smallHexRadius * (sqrt(3) * (r + q / 2.0)) + center.y;

            // Store the hexagon in the array
            hexagons[index].centerX = centerX;
            hexagons[index].centerY = centerY;
            hexagons[index].radius = smallHexRadius;
            hexagons[index].color = (Color){0,0,0,0};
			index++;
		}
	}
	
	*hexagonCount = totalHexagons;
	return hexagons;
}

void* polling_buffers(void *arg){
	Polling_args *args = (Polling_args *)arg;
	double time_per_cycle = 1.0 / CLOCK_SPEED_B;

	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 999999999/10;//time_per_cycle;

	while(1){
		if(args->index == 1){
			args->hexagon_panel->pixels[args->index].color = GREEN;
			nanosleep(&ts, NULL);
			args->hexagon_panel->pixels[args->index].color = RED;
			nanosleep(&ts, NULL);
		}
		if(args->index == 2){
			args->hexagon_panel->pixels[args->index].color = BLUE;
			nanosleep(&ts, NULL);
			args->hexagon_panel->pixels[args->index].color = YELLOW;
			nanosleep(&ts, NULL);
		}
		//TraceLog(LOG_INFO, "EXECUTE: %d", args->index);
	}
}
