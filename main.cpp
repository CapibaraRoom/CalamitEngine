#include <iostream>
#include <raylib.h>
#include "settings.hpp"

calamit::Settings settings;

int main() {
    settings.loadFromLua("lua/settings.lua");
    
    if (settings.resizable.get())
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    Vector2 winsize = settings.window_size.get();
    InitWindow(static_cast<int>(winsize.x), static_cast<int>(winsize.y),
               settings.project_name.get().c_str());
    SetTargetFPS(settings.target_fps.get());

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        int y = 30;
        int lineHeight = 30;

        int currentWidth = GetScreenWidth();
        int currentHeight = GetScreenHeight();

        DrawText(TextFormat("Project: %s", settings.project_name.get().c_str()), 20, y, 20, DARKGRAY);
        y += lineHeight;
        DrawText(TextFormat("Target FPS: %d", settings.target_fps.get()), 20, y, 20, DARKGRAY);
        y += lineHeight;
        DrawText(TextFormat("Window size: %d x %d", currentWidth, currentHeight), 20, y, 20, DARKGRAY);
        y += lineHeight;
        DrawText(TextFormat("Resizable: %s", settings.resizable.get() ? "Yes" : "No"), 20, y, 20, DARKGRAY);

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
