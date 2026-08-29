#pragma once
#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <cstdio>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

inline std::vector<std::string> search_paths;
inline void add_search_path(const std::string& p) { search_paths.push_back(p); }

inline std::string find_file(const std::string& filename) {
    for (const auto& dir : search_paths) {
        std::string full = dir + "/" + filename;
        if (FILE* f = fopen(full.c_str(), "r")) { fclose(f); return full; }
    }
    return "";
}

class EngineSettings {
    std::unordered_map<std::string, std::function<void(lua_State*, int)>> setters;
    std::unordered_map<std::string, std::function<void(lua_State*)>> getters;
public:
    template<typename T>
    void add(const char* name, T* var) {
        setters[name] = [var](lua_State* L, int i) {
            if constexpr (std::is_same_v<T, int>) *var = (int)lua_tointeger(L, i);
            else if constexpr (std::is_same_v<T, float>) *var = (float)lua_tonumber(L, i);
            else if constexpr (std::is_same_v<T, double>) *var = lua_tonumber(L, i);
            else if constexpr (std::is_same_v<T, bool>) *var = lua_toboolean(L, i) != 0;
            else if constexpr (std::is_same_v<T, std::string>) *var = luaL_optstring(L, i, "");
        };
        getters[name] = [var](lua_State* L) {
            if constexpr (std::is_same_v<T, int>) lua_pushinteger(L, *var);
            else if constexpr (std::is_same_v<T, float>) lua_pushnumber(L, *var);
            else if constexpr (std::is_same_v<T, double>) lua_pushnumber(L, *var);
            else if constexpr (std::is_same_v<T, bool>) lua_pushboolean(L, *var);
            else if constexpr (std::is_same_v<T, std::string>) lua_pushstring(L, var->c_str());
        };
    }
    bool set(const std::string& n, lua_State* L, int i) {
        auto it = setters.find(n); if (it == setters.end()) return false; it->second(L, i); return true;
    }
    bool get(const std::string& n, lua_State* L) {
        auto it = getters.find(n); if (it == getters.end()) return false; it->second(L); return true;
    }
};

inline EngineSettings engine_settings;

#define SETTINGS_PUSH(type, name, def) \
    type name = def; \
    namespace { struct _sr_##name { _sr_##name(){ engine_settings.add(#name,&name); } } _sri_##name; }

static int lua_settings_set(lua_State* L) {
    const char* n = luaL_checkstring(L, 1);
    if (engine_settings.set(n, L, 2)) return 0;
    return luaL_error(L, "unknown setting '%s'", n);
}

static int lua_settings_get(lua_State* L) {
    const char* n = luaL_checkstring(L, 1);
    if (engine_settings.get(n, L)) return 1;
    lua_pushnil(L); return 1;
}

static int lua_settings_import(lua_State* L) {
    const char* f = luaL_checkstring(L, 1);
    std::string full = find_file(f);
    if (full.empty()) return luaL_error(L, "file not found: %s", f);
    if (luaL_dofile(L, full.c_str()) != LUA_OK) { lua_pop(L, 1); }
    return 0;
}

static int lua_settings_include(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    add_search_path(path);
    lua_getglobal(L, "__engine_add_path");
    if (lua_isfunction(L, -1)) {
        lua_pushstring(L, path);
        lua_pcall(L, 1, 0, 0);
    } else {
        lua_pop(L, 1);
    }
    return 0;
}

inline void register_settings_table(lua_State* L) {
    lua_newtable(L);
    lua_pushcfunction(L, lua_settings_set);     lua_setfield(L, -2, "set");
    lua_pushcfunction(L, lua_settings_get);     lua_setfield(L, -2, "get");
    lua_pushcfunction(L, lua_settings_import);  lua_setfield(L, -2, "import");
    lua_pushcfunction(L, lua_settings_include); lua_setfield(L, -2, "include");
    lua_setglobal(L, "Settings");
}
