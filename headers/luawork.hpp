// headers/luawork.hpp
#pragma once
#include <string>
#include <cstdio>
#include "raylib.h"
#include "settings.hpp"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

// ---- Базовые функции рисования ----
static int l_draw_rectangle(lua_State* L) {
    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    int w = (int)luaL_checkinteger(L, 3);
    int h = (int)luaL_checkinteger(L, 4);
    DrawRectangle(x, y, w, h, RED);
    lua_settop(L, 0);
    return 0;
}

// ---- Загрузка файла ноды ----
static int load_node_file(lua_State* L, const std::string& path) {
    if (luaL_dofile(L, path.c_str()) != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        fprintf(stderr, "load_node_file error (%s): %s\n", path.c_str(), err ? err : "(unknown)");
        lua_pop(L, 1);
        return -1;
    }
    lua_getglobal(L, "_onload");
    int has_onload = lua_isfunction(L, -1);
    lua_pop(L, 1);
    lua_getglobal(L, "_ondraw");
    int has_ondraw = lua_isfunction(L, -1);
    lua_pop(L, 1);

    lua_newtable(L);
    lua_newtable(L);
    lua_setfield(L, -2, "__children");
    if (has_onload) {
        lua_getglobal(L, "_onload");
        lua_setfield(L, -2, "_onload");
    }
    if (has_ondraw) {
        lua_getglobal(L, "_ondraw");
        lua_setfield(L, -2, "_ondraw");
    }
    lua_pushnil(L); lua_setglobal(L, "_onload");
    lua_pushnil(L); lua_setglobal(L, "_ondraw");
    return luaL_ref(L, LUA_REGISTRYINDEX);
}

// ---- Tree.load ----
static int l_tree_load(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    int ref = load_node_file(L, path);
    if (ref >= 0) {
        lua_getglobal(L, "__roots");
        if (!lua_istable(L, -1)) {
            lua_pop(L, 1);
            lua_newtable(L);
            lua_setglobal(L, "__roots");
            lua_getglobal(L, "__roots");
        }
        int len = lua_objlen(L, -1) + 1;
        lua_pushinteger(L, ref);
        lua_rawseti(L, -2, len);
        lua_pop(L, 1);
    }
    lua_settop(L, 0);
    return 0;
}

// ---- add_child ----
static int l_add_child(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    lua_Debug ar;
    if (!lua_getstack(L, 1, &ar)) {
        luaL_error(L, "add_child: cannot determine caller");
        return 0;
    }
    lua_getinfo(L, "S", &ar);
    std::string dir = "./lua/";
    if (ar.source && ar.source[0] == '@') {
        std::string full(ar.source + 1);
        size_t pos = full.find_last_of('/');
        if (pos != std::string::npos)
            dir = full.substr(0, pos + 1);
    }
    std::string full_path = dir + path;

    lua_getglobal(L, "__current_node");
    if (!lua_istable(L, -1)) {
        luaL_error(L, "add_child: no current node");
        return 0;
    }
    lua_pushvalue(L, -1);
    int self_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    lua_pop(L, 1);

    int child_ref = load_node_file(L, full_path);
    if (child_ref < 0) {
        luaL_unref(L, LUA_REGISTRYINDEX, self_ref);
        lua_settop(L, 0);
        return 0;
    }
    lua_rawgeti(L, LUA_REGISTRYINDEX, self_ref);
    lua_getfield(L, -1, "__children");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_setfield(L, -2, "__children");
        lua_getfield(L, -1, "__children");
    }
    int len = lua_objlen(L, -1) + 1;
    lua_pushinteger(L, child_ref);
    lua_rawseti(L, -2, len);
    lua_pop(L, 2);
    luaL_unref(L, LUA_REGISTRYINDEX, self_ref);
    lua_settop(L, 0);
    return 0;
}

// ---- Рекурсивный обход ----
static void call_function(lua_State* L, int ref, const char* name) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    lua_getfield(L, -1, name);
    if (lua_isfunction(L, -1)) {
        lua_pushvalue(L, -2);
        lua_setglobal(L, "__current_node");
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            const char* err = lua_tostring(L, -1);
            fprintf(stderr, "Error in %s: %s\n", name, err ? err : "(unknown)");
            lua_pop(L, 1);
        }
        lua_pushnil(L);
        lua_setglobal(L, "__current_node");
    }
    lua_pop(L, 2);
}

static void traverse_onload(lua_State* L, int ref) {
    call_function(L, ref, "_onload");
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    lua_getfield(L, -1, "__children");
    if (lua_istable(L, -1)) {
        int n = lua_objlen(L, -1);
        for (int i = 1; i <= n; ++i) {
            lua_rawgeti(L, -1, i);
            int child = lua_tointeger(L, -1);
            lua_pop(L, 1);
            traverse_onload(L, child);
        }
    }
    lua_pop(L, 2);
}

static void traverse_ondraw(lua_State* L, int ref) {
    call_function(L, ref, "_ondraw");
    lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
    lua_getfield(L, -1, "__children");
    if (lua_istable(L, -1)) {
        int n = lua_objlen(L, -1);
        for (int i = 1; i <= n; ++i) {
            lua_rawgeti(L, -1, i);
            int child = lua_tointeger(L, -1);
            lua_pop(L, 1);
            traverse_ondraw(L, child);
        }
    }
    lua_pop(L, 2);
}

// ---- Публичные функции ----
inline void register_luawork(lua_State* L) {
    lua_register(L, "draw_rectangle", l_draw_rectangle);
    lua_newtable(L);
    lua_pushcfunction(L, l_tree_load);
    lua_setfield(L, -2, "load");
    lua_setglobal(L, "Tree");
    lua_register(L, "add_child", l_add_child);
    lua_newtable(L);
    lua_setglobal(L, "__roots");
    lua_pushnil(L);
    lua_setglobal(L, "__current_node");
}

inline lua_State* init_lua() {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    register_settings_table(L);
    register_luawork(L);
    if (luaL_dofile(L, "config.lua") != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        fprintf(stderr, "Config warning: %s\n", err ? err : "(unknown)");
        lua_pop(L, 1);
    }
    lua_settop(L, 0);
    return L;
}

inline void close_lua(lua_State* L) {
    lua_close(L);
}

inline void call_onload_all(lua_State* L) {
    lua_getglobal(L, "__roots");
    if (lua_istable(L, -1)) {
        int n = lua_objlen(L, -1);
        for (int i = 1; i <= n; ++i) {
            lua_rawgeti(L, -1, i);
            int ref = lua_tointeger(L, -1);
            lua_pop(L, 1);
            traverse_onload(L, ref);
        }
    }
    lua_pop(L, 1);
    lua_settop(L, 0);
}

inline void call_ondraw_all(lua_State* L) {
    lua_getglobal(L, "__roots");
    if (lua_istable(L, -1)) {
        int n = lua_objlen(L, -1);
        for (int i = 1; i <= n; ++i) {
            lua_rawgeti(L, -1, i);
            int ref = lua_tointeger(L, -1);
            lua_pop(L, 1);
            traverse_ondraw(L, ref);
        }
    }
    lua_pop(L, 1);
    lua_settop(L, 0);
}
