//gcc -ggdb main.c -o main -Iincludes -lraylib -lm
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <math.h>


typedef struct HexagonPanel {
	float radius;
	Vector2 position;
	struct HexagonPanel* peers_out[3];
	struct HexagonPanel* peers_in[3];
	char* in_out [6];
} HexagonPanel;

void drawPanel(float radius, Vector2 position){
	DrawPolyLines(
		position,
		6,
		radius,
		0,
		//1,
		//(Color){rand() % (255 + 1), rand() % (255 + 1), rand() % (255 + 1), 255}
		WHITE			
	);
}

int main(){
	InitWindow(1600, 900, "Test Animation");

	while(!WindowShouldClose()){
		BeginDrawing();
		float rad = 100.0;
		float half_width = sqrt(-(rad*rad/4) + rad*rad);
		
		HexagonPanel hp1 = {
			.radius = rad,
			.position = {250, 250},
		};
		
		HexagonPanel hp2 = {
			.radius = rad,
			.position = {hp1.position.x + rad * 2, hp1.position.y + rad},
		};

		HexagonPanel hp3 = {
			.radius = rad,
			.position = {hp1.position.x + rad * 2, hp1.position.y - rad},
		};
		
		hp1.peers_out[1] = &hp2;
		hp2.peers_in[1] = &hp1;
		
		hp1.peers_out[0] = &hp3;
		hp3.peers_in[0] = &hp1;
		
		hp3.peers_out[2] = &hp2;
		hp2.peers_in[2] = &hp3;

		drawPanel(hp1.radius, hp1.position);
		drawPanel(hp2.radius, hp2.position);
		drawPanel(hp3.radius, hp3.position);
		//drawHexagon(50, 50, (Vector2){200 + 50/4 + 50/2, 200 + 50/2});
		EndDrawing();
	}
}
