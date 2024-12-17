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

	//DrawText(dest, hex->center.x - hex->radius/2, hex->center.y - hex->radius/2, 6, WHITE);
	//if(hex->color.r == 255){
		//__asm__("int $3");
	//}
	DrawText(index_as_str, hex->center.x - hex->radius/2, hex->center.y - hex->radius/2, 6, WHITE);
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

void process_rgb_values(HexagonPanel* hp, Frame *frame){
	for(int i = 0; i < 127; i++){
		RGB_Value frame_color = frame->rgb_value[i];
		hp->pixels[i].color = (Color){ frame_color.r, frame_color.g, frame_color.b, 255};
		//if(i == 66 && (frame_color.r != 0 | frame_color.g != 0 | frame_color.b != 0)){
		//	__asm__("int $3");
		//}
	}
}
