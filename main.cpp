#include "raylib.h"
#include "settings.hpp"

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

    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    lua_register(L, "set_setting", l_set_setting);
    lua_register(L, "importSettings", l_importSettings);

    if (luaL_dofile(L, "config.lua") != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        fprintf(stderr, "Config warning: %s\n", err ? err : "(unknown)");
        lua_pop(L, 1);
    }

    SetConfigFlags(windowResizable ? FLAG_WINDOW_RESIZABLE : 0);
    InitWindow(windowWidth, windowHeight, windowTitle.c_str());
    SetTargetFPS(targetFPS);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText(("Title: " + windowTitle).c_str(), 10, 10, 20, BLACK);
        char buf[64];
        sprintf(buf, "Size: %dx%d", windowWidth, windowHeight);
        DrawText(buf, 10, 35, 20, BLACK);
        sprintf(buf, "Target FPS: %d", targetFPS);
        DrawText(buf, 10, 60, 20, BLACK);
        DrawText(windowResizable ? "Resizable: ON" : "Resizable: OFF", 10, 85, 20, BLACK);
        DrawFPS(10, 110);

        EndDrawing();
    }

    lua_close(L);
    CloseWindow();
    return 0;
}
