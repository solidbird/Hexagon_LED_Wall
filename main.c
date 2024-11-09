#include <pthread.h>
#include "timer.h"
#include "hexagon.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

Camera2D camera_setup(){
	Camera2D camera = { 0 };
	camera.target = (Vector2) {SCREEN_WIDTH, SCREEN_HEIGHT};
	camera.offset = (Vector2) {SCREEN_WIDTH, SCREEN_HEIGHT};
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	return camera;
}

int main(int argc, char** argv) {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Indexed Hexagon Pixels in Panel");
	SetTargetFPS(60);
	//startTimer(&timer, 999);

	Camera2D camera = camera_setup();
    // Variable to store the total number of small hexagons
    int hexagonCount = 0;
    
    // Prepare the Hexagon panels with their position relativ to other and generate "pixels" 
	HexagonPanel hp[5];
	Vector2 last_panel_center = {0};

	hp[0] = (HexagonPanel){
		.radius = largeHexRadius,
		.center.x = 250,
		.center.y = 250,
	};
	hp[0].pixels = generateHexagons((Vector2){hp[0].center.x, hp[0].center.y}, &(hp[0].hexagonCount)),
	last_panel_center = hp[0].center;
	
	hp[1] = (HexagonPanel){
		.radius = largeHexRadius,
		.center.x = last_panel_center.x + dock_bottom_right.x,
		.center.y = last_panel_center.y + dock_bottom_right.y,
	};
	hp[1].pixels = generateHexagons((Vector2){hp[1].center.x, hp[1].center.y}, &(hp[1].hexagonCount)),
	last_panel_center = hp[1].center;

	hp[2] = (HexagonPanel){
		.radius = largeHexRadius,
		.center.x = last_panel_center.x + dock_bottom_right.x,
		.center.y = last_panel_center.y + dock_bottom_right.y,
	};
	hp[2].pixels = generateHexagons((Vector2){hp[2].center.x, hp[2].center.y}, &(hp[2].hexagonCount)),
	last_panel_center = hp[2].center;

	hp[3] = (HexagonPanel){
		.radius = largeHexRadius,
		.center.x = last_panel_center.x + dock_top_right.x,
		.center.y = last_panel_center.y + dock_top_right.y,
	};
	hp[3].pixels = generateHexagons((Vector2){hp[3].center.x, hp[3].center.y}, &(hp[3].hexagonCount)),
	last_panel_center = hp[3].center;
	
	hp[4] = (HexagonPanel){
		.radius = largeHexRadius,
		.center.x = last_panel_center.x + dock_top_left.x,
		.center.y = last_panel_center.y + dock_top_left.y,
	};
	hp[4].pixels = generateHexagons((Vector2){hp[4].center.x, hp[4].center.y}, &(hp[4].hexagonCount)),

	//Hook up the connections from each panel with eachother
	hp[0].peer_out[1] = &hp[4];
	hp[4].peer_in[1] = &hp[0];
	hp[0].peer_out[2] = &hp[1];
	hp[1].peer_in[2] = &hp[0];

	hp[1].peer_out[0] = &hp[4];
	hp[4].peer_in[0] = &hp[1];
	hp[1].peer_out[1] = &hp[3];
	hp[3].peer_in[1] = &hp[1];
	hp[1].peer_out[2] = &hp[2];
	hp[2].peer_in[2] = &hp[1];

	hp[2].peer_out[0] = &hp[3];
	hp[3].peer_in[0] = &hp[2];

	hp[4].peer_out[2] = &hp[3];
	hp[3].peer_in[2] = &hp[4];
	//... and more not connected all yet
	
	//since all panels have the same size the hexagonCount need to be saved once from one panel

	Polling_args *hp_args[5];
	for(int i = 0; i < 5; i++){
		hp_args[i] = (Polling_args*) malloc(sizeof(Polling_args));
		hp_args[i]->hexagon_panel = &hp[i];
		hp_args[i]->index = i;

		for(int x = 0; x < 3; x++){
			init_buffer(&(hp[i].buffer_out[x]));
			init_buffer(&(hp[i].buffer_in[x]));
		}
		
		pthread_create(&(hp[i].thread), NULL, polling_buffers, hp_args[i]);
	}

	while(!WindowShouldClose()){
		ClearBackground(BLACK);
		BeginDrawing();

		if(IsKeyDown(KEY_W)) camera.target.y -= 5.0f;
		if(IsKeyDown(KEY_S)) camera.target.y += 5.0f;
		if(IsKeyDown(KEY_A)) camera.target.x -= 5.0f;
		if(IsKeyDown(KEY_D)) camera.target.x += 5.0f;
		if(IsKeyDown(KEY_PERIOD)) camera.zoom += 0.01f;
		if(IsKeyDown(KEY_COMMA)) camera.zoom -= 0.01f;

		BeginMode2D(camera);
    	// Access and draw each hexagon from the array
		for(int x = 0; x < 5; x++){
	    	for (int i = 0; i < 127; ++i) {
				drawHexagon(&(hp[x].pixels[i]), i);
			}
		}
		EndMode2D();
		EndDrawing();
	}

    // Free the dynamically allocated memory
	for(int i = 0; i < 5; i++){
    	free(hp[i].pixels);
		pthread_join(hp[i].thread, NULL);
	}
	
	return 0;
}
