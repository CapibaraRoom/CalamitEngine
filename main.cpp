#include <iostream>
#include <raylib.h>

#include "settings.hpp"

calamit::Settings settings;

int main() {
    if (settings.getResizable())
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    
    auto winsize = settings.getWindowSize();
    InitWindow(winsize.x, winsize.y, 
               settings.getProjectName().c_str());
    SetTargetFPS(settings.getTargetFps());

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
