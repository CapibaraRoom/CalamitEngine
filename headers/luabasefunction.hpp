#pragma once
#include "raylib.h"
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

static int l_draw_rectangle(lua_State* L) {
    DrawRectangle(
        (int)luaL_checkinteger(L, 1), (int)luaL_checkinteger(L, 2),
        (int)luaL_checkinteger(L, 3), (int)luaL_checkinteger(L, 4), RED);
    return 0;
}

inline void register_base_functions(lua_State* L) {
    lua_register(L, "draw_rectangle", l_draw_rectangle);
}
