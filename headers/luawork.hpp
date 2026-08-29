#pragma once
#include "settings.hpp"
#include "luabasefunction.hpp"
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

inline void call_tree_update(lua_State* L) {
    lua_getglobal(L, "Tree");
    lua_getfield(L, -1, "update");
    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        fprintf(stderr, "[Calamit] Tree.update error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

inline void call_tree_draw(lua_State* L) {
    lua_getglobal(L, "Tree");
    lua_getfield(L, -1, "draw");
    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        fprintf(stderr, "[Calamit] Tree.draw error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);
}

inline lua_State* init_lua() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    register_base_functions(L);
    register_settings_table(L);

    if (luaL_dofile(L, "engine.lua") != LUA_OK) {
        fprintf(stderr, "[Calamit] engine error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    if (luaL_dofile(L, "config.lua") != LUA_OK) {
        fprintf(stderr, "[Calamit] config error: %s\n", lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    lua_settop(L, 0);
    return L;
}

inline void close_lua(lua_State* L) { lua_close(L); }
