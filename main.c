#include <pthread.h>
#include "timer.h"
#include "hexagon.h"

void process_stuff(HexagonPanel* hp, int index, int is_active){
	hp->pixels[index].color = BLUE;
	//hp->pixels[index].pixel_color = (Color){0, 0, 0, 0};
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
		.pixels = generateHexagons((Vector2){hp1.centerX, hp1.centerY}, &(hp1.hexagonCount)),
	};
	
	HexagonPanel hp2 = {
		.radius = largeHexRadius,
		.centerX = hp1.centerX + dock_bottom_right.x,
		.centerY = hp1.centerY + dock_bottom_right.y,
		.pixels = generateHexagons((Vector2){hp2.centerX, hp2.centerY}, &(hp2.hexagonCount)),
	};

	HexagonPanel hp3 = {
		.radius = largeHexRadius,
		.centerX = hp2.centerX + dock_bottom_right.x,
		.centerY = hp2.centerY + dock_bottom_right.y,
		.pixels = generateHexagons((Vector2){hp3.centerX, hp3.centerY}, &(hp3.hexagonCount)),
	};

	HexagonPanel hp4 = {
		.radius = largeHexRadius,
		.centerX = hp3.centerX + dock_top_right.x,
		.centerY = hp3.centerY + dock_top_right.y,
		.pixels = generateHexagons((Vector2){hp4.centerX, hp4.centerY}, &(hp4.hexagonCount)),
	};
	
	HexagonPanel hp5 = {
		.radius = largeHexRadius,
		.centerX = hp4.centerX + dock_top_left.x,
		.centerY = hp4.centerY + dock_top_left.y,
		.pixels = generateHexagons((Vector2){hp5.centerX, hp5.centerY}, &(hp5.hexagonCount)),
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

	Polling_args* hp1_args = (Polling_args*) malloc(sizeof(Polling_args));
	hp1_args->hexagon_panel = &hp1;
	hp1_args->index = 1;
	
	Polling_args* hp2_args = (Polling_args*) malloc(sizeof(Polling_args));
	hp2_args->hexagon_panel = &hp2;
	hp2_args->index = 2;
	
	Polling_args* hp3_args = (Polling_args*) malloc(sizeof(Polling_args));
	hp3_args->hexagon_panel = &hp3;
	hp3_args->index = 3;
	
	Polling_args* hp4_args = (Polling_args*) malloc(sizeof(Polling_args));
	hp4_args->hexagon_panel = &hp4;
	hp4_args->index = 4;

	Polling_args* hp5_args = (Polling_args*) malloc(sizeof(Polling_args));
	hp5_args->hexagon_panel = &hp5;
	hp5_args->index = 5;

	pthread_create(&(hp1.thread), NULL, polling_buffers, hp1_args);
	pthread_create(&(hp2.thread), NULL, polling_buffers, hp2_args);
	pthread_create(&(hp3.thread), NULL, polling_buffers, hp3_args);
	pthread_create(&(hp4.thread), NULL, polling_buffers, hp4_args);
	pthread_create(&(hp5.thread), NULL, polling_buffers, hp5_args);

	while(!WindowShouldClose()){
		BeginDrawing();
    	// Access and draw each hexagon from the array
    	for (int i = 0; i < hexagonCount; ++i) {
			drawHexagon(hp1.pixels[i], i);
			drawHexagon(hp2.pixels[i], i);
			drawHexagon(hp3.pixels[i], i);
			drawHexagon(hp4.pixels[i], i);
			drawHexagon(hp5.pixels[i], i);
			
		}
		EndDrawing();
	}

    // Free the dynamically allocated memory
    free(hp1.pixels);
    free(hp2.pixels);
    free(hp3.pixels);
    free(hp4.pixels);
    free(hp5.pixels);

	pthread_join(hp1.thread, NULL);
	pthread_join(hp2.thread, NULL);
	pthread_join(hp3.thread, NULL);
	pthread_join(hp4.thread, NULL);
	pthread_join(hp5.thread, NULL);

    return 0;
}
