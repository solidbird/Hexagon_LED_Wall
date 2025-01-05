#include "timer.h"
#include "hexagon.h"
#include "controller.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 900

#define connect_panels(panel_parent, panel_child, edge_index) \
	panel_parent.peer_out[edge_index] = &panel_child;\
	panel_child.peer_in[edge_index] = &panel_parent;\

Camera2D camera_setup(){
	Camera2D camera = { 0 };
	camera.target = (Vector2) {SCREEN_WIDTH, SCREEN_HEIGHT};
	camera.offset = (Vector2) {SCREEN_WIDTH, SCREEN_HEIGHT};
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	return camera;
}

pthread_t controller_thread;

int main() {
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Indexed Hexagon Pixels in Panel");
	SetTargetFPS(60);
	//startTimer(&timer, 999);

	Camera2D camera = camera_setup();
    
    // Prepare the Hexagon panels with their position relativ to other and generate "pixels" 
	HexagonPanel hp[5];
	Vector2 last_panel_center = {0};

	hp[0] = (HexagonPanel){
		.radius = largeHexRadius,
		.center.x = 250,
		.center.y = 250,
	};
	hp[0].index = 0;
	hp[0].pixels = generateHexagons((Vector2){hp[0].center.x, hp[0].center.y}, &(hp[0].hexagonCount)),
	last_panel_center = hp[0].center;
	
	hp[1] = (HexagonPanel){
		.radius = largeHexRadius,
		.center.x = last_panel_center.x + dock_bottom_right.x,
		.center.y = last_panel_center.y + dock_bottom_right.y,
	};
	hp[1].index = 1;
	hp[1].pixels = generateHexagons((Vector2){hp[1].center.x, hp[1].center.y}, &(hp[1].hexagonCount)),
	last_panel_center = hp[1].center;

	hp[2] = (HexagonPanel){
		.radius = largeHexRadius,
		.center.x = last_panel_center.x + dock_bottom_right.x,
		.center.y = last_panel_center.y + dock_bottom_right.y,
	};
	hp[2].index = 2;
	hp[2].pixels = generateHexagons((Vector2){hp[2].center.x, hp[2].center.y}, &(hp[2].hexagonCount)),
	last_panel_center = hp[2].center;

	hp[3] = (HexagonPanel){
		.radius = largeHexRadius,
		.center.x = last_panel_center.x + dock_top_right.x,
		.center.y = last_panel_center.y + dock_top_right.y,
	};
	hp[3].index = 3;
	hp[3].pixels = generateHexagons((Vector2){hp[3].center.x, hp[3].center.y}, &(hp[3].hexagonCount)),
	last_panel_center = hp[3].center;
	
	hp[4] = (HexagonPanel){
		.radius = largeHexRadius,
		.center.x = last_panel_center.x + dock_top_left.x,
		.center.y = last_panel_center.y + dock_top_left.y,
	};
	hp[4].index = 4;
	hp[4].pixels = generateHexagons((Vector2){hp[4].center.x, hp[4].center.y}, &(hp[4].hexagonCount)),

	//Hook up the connections from each panel with eachother
	connect_panels(hp[0], hp[4], 1);
	connect_panels(hp[0], hp[1], 2);
	
	connect_panels(hp[1], hp[4], 0);
	connect_panels(hp[1], hp[3], 1);
	connect_panels(hp[1], hp[2], 2);

	connect_panels(hp[2], hp[3], 0);

	connect_panels(hp[4], hp[3], 2);

	HexagonPanel* master_hp = (HexagonPanel*) malloc(sizeof(HexagonPanel));
	master_hp->index = -1;
	for(int x = 0; x < 3; x++){
		master_hp->buffer_in[x] = ring_buffer_init(1000);
		master_hp->buffer_out[x] = ring_buffer_init(1000);
	}

	connect_panels((*master_hp), hp[0], 1);

	Polling_args *master_args = (Polling_args*) malloc(sizeof(Polling_args));
	master_args->hexagon_panel = master_hp;

	Polling_args *hp_args[5];
	for(int i = 0; i < 5; i++){
		for(int x = 0; x < 3; x++){
			hp_args[i] = (Polling_args*) malloc(sizeof(Polling_args));
			hp_args[i]->hexagon_panel = &hp[i];
			hp_args[i]->buffer_in_index = x;

			hp[i].buffer_out[x] = ring_buffer_init(10000);
			hp[i].buffer_in[x] = ring_buffer_init(10000);
		}
		//pthread_create(&(hp[i].thread), NULL, node_main, hp_args[i]);
	}

	Controller_args controller_args = {
		.nodes = hp,
		.master = master_hp,
		.nodes_amount = 5
	};
	pthread_create(&controller_thread, NULL, controller_main, &controller_args);

	while(!WindowShouldClose()){
		ClearBackground(BLACK);
		BeginDrawing();
		BeginMode2D(camera);

		if(IsKeyDown(KEY_W)) camera.target.y -= 5.0f;
		if(IsKeyDown(KEY_S)) camera.target.y += 5.0f;
		if(IsKeyDown(KEY_A)) camera.target.x -= 5.0f;
		if(IsKeyDown(KEY_D)) camera.target.x += 5.0f;
		if(IsKeyDown(KEY_PERIOD)) camera.zoom += 0.01f;
		if(IsKeyDown(KEY_COMMA)) camera.zoom -= 0.01f;

		Vector2 mous_pos = GetScreenToWorld2D(GetMousePosition(), camera);
    	// Access and draw each hexagon from the array
		for(int x = 0; x < 5; x++){
	    	for (int i = 0; i < 127; ++i) {
				drawHexagon(&(hp[x].pixels[i]), i);
				DrawCircle(mous_pos.x, mous_pos.y, 20.0, RED);
			}
		}
		EndMode2D();
		EndDrawing();
	}

    // Free the dynamically allocated memory
	for(int i = 0; i < 5; i++){
    	free(hp[i].pixels);
	}
	
	return 0;
}
