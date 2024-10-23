#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <raylib.h>

#define M_PI 3.14159265358979323846

#define largeHexRadius 60.0  // Radius of the large hexagon
#define smallHexRadius 10.0   // Radius of the smaller hexagons

// Structure to represent a hexagon
typedef struct {
    float centerX;  // X coordinate of the hexagon center
    float centerY;  // Y coordinate of the hexagon center
    float radius;   // Radius of the hexagon
	Color pixel_color;
} Hexagon;

typedef struct HexagonPanel {
	float centerX;
	float centerY;
	float radius;
	int hexagonCount;
	struct HexagonPanel *peer_out[3];
	struct HexagonPanel *peer_in[3];
	Hexagon *hexagon_pixels;
} HexagonPanel;

void drawHexagon(Hexagon hex){
	DrawPolyLines(
		(Vector2){hex.centerX, hex.centerY},
		6,
		hex.radius,
		0,
		WHITE	
	);

	DrawCircleV(
		(Vector2){hex.centerX, hex.centerY},
		hex.radius*0.80,
		hex.pixel_color
	);
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
            hexagons[index].pixel_color = (Color){0,0,0,0};
            index++;
        }
    }

    *hexagonCount = totalHexagons;
    return hexagons;
}

int main() {

	InitWindow(1600, 900, "Indexed Hexagon Pixels in Panel");

    // Variable to store the total number of small hexagons
    int hexagonCount = 0;
    
    // Generate the hexagons and store them in an array
	HexagonPanel hp1 = {
		.radius = largeHexRadius,
		.centerX = 250,
		.centerY = 250,
		.hexagon_pixels = generateHexagons((Vector2){hp1.centerX, hp1.centerY}, &(hp1.hexagonCount)),
	};
	
	HexagonPanel hp2 = {
		.radius = largeHexRadius,
		.centerX = hp1.centerX + (largeHexRadius * 2 - smallHexRadius),
		.centerY = hp1.centerY + (largeHexRadius * 3 - smallHexRadius),
		.hexagon_pixels = generateHexagons((Vector2){hp2.centerX, hp2.centerY}, &(hp2.hexagonCount)),
	};

	HexagonPanel hp3 = {
		.radius = largeHexRadius,
		.centerX = hp1.centerX - (largeHexRadius * 2 - smallHexRadius),
		.centerY = hp1.centerY + (largeHexRadius * 3 - smallHexRadius),
		.hexagon_pixels = generateHexagons((Vector2){hp3.centerX, hp3.centerY}, &(hp3.hexagonCount)),
	};

	HexagonPanel hp4 = {
		.radius = largeHexRadius,
		.centerX = hp2.centerX - (largeHexRadius * 2 - smallHexRadius),
		.centerY = hp2.centerY + (largeHexRadius * 3 - smallHexRadius),
		.hexagon_pixels = generateHexagons((Vector2){hp4.centerX, hp4.centerY}, &(hp4.hexagonCount)),
	};

	hp1.peer_out[1] = &hp2;
	hp2.peer_in[1] = &hp1;

	hexagonCount = hp1.hexagonCount;

    // Access and draw each hexagon from the array
	while(!WindowShouldClose()){
		BeginDrawing();
    	for (int i = 0; i < hexagonCount; ++i) {
			if(i <= 42){
				hp1.hexagon_pixels[i].pixel_color = RED;
				hp2.hexagon_pixels[i].pixel_color = RED;
			}
			drawHexagon(hp1.hexagon_pixels[i]);
			drawHexagon(hp2.hexagon_pixels[i]);
			drawHexagon(hp3.hexagon_pixels[i]);
			drawHexagon(hp4.hexagon_pixels[i]);
		}
		EndDrawing();
	}

    // Free the dynamically allocated memory
    free(hp1.hexagon_pixels);
    free(hp2.hexagon_pixels);
    free(hp3.hexagon_pixels);

    return 0;
}

