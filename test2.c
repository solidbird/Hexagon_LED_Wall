#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <raylib.h>

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

/* Actual implementation

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
*/

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
		hex.radius*0.80,
		hex.pixel_color
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
            hexagons[index].pixel_color = (Color){0,0,0,0};
            index++;
        }
    }

    *hexagonCount = totalHexagons;
    return hexagons;
}

void process_stuff(HexagonPanel* hp, int index, int is_active){
	if(is_active){
		hp->hexagon_pixels[index].pixel_color = BLUE;
	}else{
		hp->hexagon_pixels[index].pixel_color = (Color){0, 0, 0, 0};
	}
}

int main() {

	InitWindow(1600, 900, "Indexed Hexagon Pixels in Panel");

    // Variable to store the total number of small hexagons
    int hexagonCount = 0;
    
    // Prepare the Hexagon panels with their position relativ to other and generate "pixels" 
	HexagonPanel hp1 = {
		.radius = largeHexRadius,
		.centerX = 250,
		.centerY = 250,
		.hexagon_pixels = generateHexagons((Vector2){hp1.centerX, hp1.centerY}, &(hp1.hexagonCount)),
	};
	
	HexagonPanel hp2 = {
		.radius = largeHexRadius,
		.centerX = hp1.centerX + dock_bottom_right.x,
		.centerY = hp1.centerY + dock_bottom_right.y,
		.hexagon_pixels = generateHexagons((Vector2){hp2.centerX, hp2.centerY}, &(hp2.hexagonCount)),
	};

	HexagonPanel hp3 = {
		.radius = largeHexRadius,
		.centerX = hp2.centerX + dock_bottom_right.x,
		.centerY = hp2.centerY + dock_bottom_right.y,
		.hexagon_pixels = generateHexagons((Vector2){hp3.centerX, hp3.centerY}, &(hp3.hexagonCount)),
	};

	HexagonPanel hp4 = {
		.radius = largeHexRadius,
		.centerX = hp3.centerX + dock_top_right.x,
		.centerY = hp3.centerY + dock_top_right.y,
		.hexagon_pixels = generateHexagons((Vector2){hp4.centerX, hp4.centerY}, &(hp4.hexagonCount)),
	};
	
	HexagonPanel hp5 = {
		.radius = largeHexRadius,
		.centerX = hp4.centerX + dock_top_left.x,
		.centerY = hp4.centerY + dock_top_left.y,
		.hexagon_pixels = generateHexagons((Vector2){hp5.centerX, hp5.centerY}, &(hp5.hexagonCount)),
	};

	//Hook up the connections from each panel with eachother
	hp1.peer_out[2] = &hp2;
	hp2.peer_in[2] = &hp1;

	hp2.peer_out[2] = &hp3;
	hp3.peer_in[2] = &hp2;
	
	hp3.peer_out[0] = &hp4;
	hp4.peer_in[0] = &hp3;
	//... and more not connected all yet
	
	//since all panels have the same size the hexagonCount need to be saved once from one panel
	hexagonCount = hp1.hexagonCount;

	while(!WindowShouldClose()){
		BeginDrawing();
    	// Access and draw each hexagon from the array
    	for (int i = 0; i < hexagonCount; ++i) {
			process_stuff(&hp1, 7, 1);	
			process_stuff(&hp1, 10, 1);
			process_stuff(&hp3, 42, 1);	
			process_stuff(&hp3, 69, 1);
	
			drawHexagon(hp1.hexagon_pixels[i], i);
			drawHexagon(hp2.hexagon_pixels[i], i);
			drawHexagon(hp3.hexagon_pixels[i], i);
			drawHexagon(hp4.hexagon_pixels[i], i);
			drawHexagon(hp5.hexagon_pixels[i], i);
		}
		EndDrawing();
	}

    // Free the dynamically allocated memory
    free(hp1.hexagon_pixels);
    free(hp2.hexagon_pixels);
    free(hp3.hexagon_pixels);
    free(hp4.hexagon_pixels);
    free(hp5.hexagon_pixels);

    return 0;
}

