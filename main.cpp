#include "raylib.h"
#include "settings.hpp"
#include "luawork.hpp"

SETTINGS_PUSH(int, windowWidth, 800);
SETTINGS_PUSH(int, windowHeight, 600);
SETTINGS_PUSH(int, targetFPS, 60);
SETTINGS_PUSH(bool, windowResizable, false);
SETTINGS_PUSH(std::string, windowTitle, "Calamit");

int main() {

    lua_State* L = init_lua();

    SetConfigFlags(windowResizable ? FLAG_WINDOW_RESIZABLE : 0);
    InitWindow(windowWidth, windowHeight, windowTitle.c_str());
    SetTargetFPS(targetFPS);

    call_onload_all(L);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        call_ondraw_all(L);

        char buf[64];
        DrawText(("Title: " + windowTitle).c_str(), 10, 10, 20, BLACK);
        sprintf(buf, "Size: %dx%d", windowWidth, windowHeight);
        DrawText(buf, 10, 35, 20, BLACK);
        sprintf(buf, "Target FPS: %d", targetFPS);
        DrawText(buf, 10, 60, 20, BLACK);
        DrawText(windowResizable ? "Resizable: ON" : "Resizable: OFF", 10, 85, 20, BLACK);
        DrawFPS(10, 110);

        EndDrawing();
    }

    close_lua(L);
    CloseWindow();
    return 0;
}
