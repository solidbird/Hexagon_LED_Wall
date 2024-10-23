#include <stdio.h>
#include <math.h>
#include <raylib.h>

#define M_PI 3.14159265358979323846

// Function to draw a single hexagon by printing its vertices
/*void drawHexagon(float centerX, float centerY, float radius) {
    printf("Hexagon center: (%f, %f)\n", centerX, centerY);
    for (int i = 0; i < 6; ++i) {
        float angle = 2 * M_PI * i / 6;  // Angle for each vertex
        float x = centerX + radius * cos(angle);
        float y = centerY + radius * sin(angle);
        printf("Vertex %d: (%f, %f)\n", i + 1, x, y);
    }
    printf("\n");
}*/

void drawHexagon(float centerX, float centerY, float radius) {
	DrawPolyLines(
		(Vector2){centerX, centerY},
		6,
		radius,
		0,
		WHITE
	);
}

// Function to place hexagons in a large hexagonal pattern
void drawHexagonsWithinLargeHex(Vector2 center, float largeRadius, float smallRadius) {
    int maxDistance = (int)(largeRadius / smallRadius);  // Number of layers

    for (int q = -maxDistance; q <= maxDistance; ++q) {
        int r1 = fmax(-maxDistance, -q - maxDistance);
        int r2 = fmin(maxDistance, -q + maxDistance);
        for (int r = r1; r <= r2; ++r) {
            // Calculate center of the hexagon
            float centerX = smallRadius * (3.0f / 2.0f * q) + center.x;
            float centerY = smallRadius * (sqrt(3) * (r + q / 2.0f)) + center.y;

            // Draw the small hexagon at (centerX, centerY)
            drawHexagon(centerX, centerY, smallRadius);
        }
    }
}

int main() {
    float largeHexRadius = 100.0f;  // Large hexagon radius
    float smallHexRadius = 10.0f;   // Small hexagons radius

	InitWindow(1600, 900, "Test hex draw alg");

	while(!WindowShouldClose()){
		BeginDrawing();
		drawHexagonsWithinLargeHex((Vector2){200,200},largeHexRadius, smallHexRadius);
		drawHexagonsWithinLargeHex((Vector2){200,600},largeHexRadius, smallHexRadius);
		drawHexagonsWithinLargeHex((Vector2){600,200},largeHexRadius, smallHexRadius);
		drawHexagonsWithinLargeHex((Vector2){600,600},largeHexRadius, smallHexRadius);
		EndDrawing();
	}
    return 0;
}
