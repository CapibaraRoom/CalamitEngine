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

    call_tree_update(L);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        call_tree_draw(L);
        DrawFPS(10, 10);
        EndDrawing();
    }

    close_lua(L);
    CloseWindow();
    return 0;
}
