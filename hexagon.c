
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
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



void drawHexagon(Hexagon* hex, int index){
	DrawPolyLines(
		(Vector2){hex->center.x, hex->center.y},
		6,
		hex->radius,
		0,
		WHITE	
	);

	DrawCircleV(
		(Vector2){hex->center.x, hex->center.y},
		hex->radius* 0.85,
		hex->color
	);

	char index_as_str[5];
	sprintf(index_as_str, "%d", index);	
	
	char coord_r[5];
	char coord_q[5];
	char dest[12];

	sprintf(coord_r, "%d", hex->coordinates.r);
	sprintf(coord_q, "%d", hex->coordinates.q);
	strcpy(dest, coord_r);
	strcat(dest, ",");
	strcat(dest, coord_q);
	
	DrawText(dest, hex->center.x - hex->radius/2, hex->center.y - hex->radius/2, 6, WHITE);
	//DrawText(index_as_str, hex.center.x - hex.radius/2, hex.center.y - hex.radius/2, 6, WHITE);
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
            // Store the hexagon in the array
            hexagons[index].center.x = smallHexRadius * (3.0 / 2.0 * q) + center.x;
            hexagons[index].center.y = smallHexRadius * (sqrt(3) * (r + q / 2.0)) + center.y;
            hexagons[index].radius = smallHexRadius;
            hexagons[index].color = (Color){0,0,0,0};
			hexagons[index].coordinates.r = r;
			hexagons[index].coordinates.q = q;
			index++;
		}
	}
	
	*hexagonCount = totalHexagons;
	return hexagons;
}

void process_stuff(HexagonPanel* hp, int index){
	double time_per_cycle = 1.0 / CLOCK_SPEED_B;

	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 999999999/60;//time_per_cycle;
	//hp->pixels[index].pixel_color = (Color){0, 0, 0, 0};
	hp->pixels[index].color = GREEN;
	nanosleep(&ts, NULL);
	hp->pixels[index].color = BLACK;
	//nanosleep(&ts, NULL);
}

void* polling_buffers(void *arg){
	Polling_args *args = (Polling_args *)arg;
	//TODO: Mostlikely the sending is faster than the popping so segmentation fault in threads
	//maybe try to not generate data per thread but instead start building master that sends it's own
	//data through the threads and just pass along the information to the next panel

	//or

	//try to fix this segmentation fault first by just looking whats actually going on but in this case way harder
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 50;//time_per_cycle;

	while(1){
		//Pull for recieving data in input_buffer
		for(int x = 0; x < 3; x++){
			if(args->hexagon_panel->peer_in[x] == NULL){ continue; }
			if(is_empty(&(args->hexagon_panel->buffer_in[x]))){ continue; }

			for(int i = 0; i < 5; i++){
				//Do something with the data on own panel. PROCESS!!
				process_stuff(args->hexagon_panel, pop(&(args->hexagon_panel->buffer_in[x])));
			}
		}

		//Pull for sending data by sending it to the output peers input buffer
		for(int x = 0; x < 3; x++){
			if(args->hexagon_panel->peer_out[x] == NULL){ continue; }
			for(int i = 0; i < 100; i++){
				push(&(args->hexagon_panel->peer_out[x]->buffer_in[x]), i);
			}
		}

		nanosleep(&ts, NULL);
		//TraceLog(LOG_INFO, "EXECUTE: %d", args->index);
	}
}
