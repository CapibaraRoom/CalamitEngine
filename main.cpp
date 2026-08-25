#include "raylib.h"
#include "settings.hpp"
#include "luawork.hpp"

int windowWidth = 800;
int windowHeight = 600;
int targetFPS = 60;
bool windowResizable = false;
std::string windowTitle = "Calamit";

void registerSettings() {
    ADD_SETTING(windowWidth);
    ADD_SETTING(windowHeight);
    ADD_SETTING(targetFPS);
    ADD_SETTING(windowResizable);
    ADD_SETTING(windowTitle);
}

int main() {
    registerSettings();

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
