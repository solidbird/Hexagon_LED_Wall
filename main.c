#include "timer.h"
#include "hexagon.h"

void process_stuff(HexagonPanel* hp, int index, int is_active){
	hp->hexagon_pixels[index].pixel_color = BLUE;
	//hp->hexagon_pixels[index].pixel_color = (Color){0, 0, 0, 0};
}

int main() {
	InitWindow(1600, 900, "Indexed Hexagon Pixels in Panel");
	//startTimer(&timer, 999);

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

