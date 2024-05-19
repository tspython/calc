#include "raylib.h"
#include "raygui.h"
#include "graph.h"
#include "gui.h"

int main() {
    InitWindow(800, 450, "Graphing Calculator");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Update and draw GUI
        UpdateGui();
        DrawGui();

        // Update and draw Graph
        UpdateGraph();
        DrawGraph();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

