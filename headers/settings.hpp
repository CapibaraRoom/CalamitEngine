#pragma once
#include <unordered_map>
#include <string>
#include <functional>
#include <cstdio>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

class SettingsRegistry {
    std::unordered_map<std::string, std::function<void(lua_State*, int)>> setters;
    std::unordered_map<std::string, std::function<void(lua_State*)>> getters;
public:
    template<typename T>
    void add(const char* name, T* var) {
        setters[name] = [var](lua_State* L, int idx) {
            if constexpr (std::is_same_v<T, int>) {
                *var = static_cast<T>(lua_tointeger(L, idx));
            } else if constexpr (std::is_same_v<T, float>) {
                *var = static_cast<float>(lua_tonumber(L, idx));
            } else if constexpr (std::is_same_v<T, double>) {
                *var = lua_tonumber(L, idx);
            } else if constexpr (std::is_same_v<T, bool>) {
                *var = lua_toboolean(L, idx) != 0;
            } else if constexpr (std::is_same_v<T, std::string>) {
                const char* s = lua_tostring(L, idx);
                *var = s ? s : "";
            } else {
                static_assert(sizeof(T) == 0, "Unsupported type");
            }
        };
        getters[name] = [var](lua_State* L) {
            if constexpr (std::is_same_v<T, int>) {
                lua_pushinteger(L, *var);
            } else if constexpr (std::is_same_v<T, float>) {
                lua_pushnumber(L, *var);
            } else if constexpr (std::is_same_v<T, double>) {
                lua_pushnumber(L, *var);
            } else if constexpr (std::is_same_v<T, bool>) {
                lua_pushboolean(L, *var);
            } else if constexpr (std::is_same_v<T, std::string>) {
                lua_pushstring(L, var->c_str());
            } else {
                static_assert(sizeof(T) == 0, "Unsupported type");
            }
        };
    }

    bool set(const std::string& name, lua_State* L, int idx) {
        auto it = setters.find(name);
        if (it != setters.end()) {
            it->second(L, idx);
            return true;
        }
        return false;
    }

    bool get(const std::string& name, lua_State* L) {
        auto it = getters.find(name);
        if (it != getters.end()) {
            it->second(L);
            return true;
        }
        return false;
    }
};

inline SettingsRegistry g_settings;
#define ADD_SETTING(var) g_settings.add(#var, &var)

// ---- Lua-функции для таблицы Settings ----
static int l_settings_set(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    if (g_settings.set(name, L, 2))
        return 0;
    return luaL_error(L, "Unknown setting: %s", name);
}

static int l_settings_get(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    if (g_settings.get(name, L))
        return 1;
    lua_pushnil(L);
    return 1;
}

static int l_settings_import(lua_State* L) {
    const char* path = luaL_checkstring(L, 1);
    if (luaL_dofile(L, path) != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        fprintf(stderr, "Failed to load settings: %s\n", err ? err : "(unknown)");
        lua_pop(L, 1);
    }
    return 0;
}

inline void register_settings_table(lua_State* L) {
    lua_newtable(L);
    lua_pushcfunction(L, l_settings_set);
    lua_setfield(L, -2, "set");
    lua_pushcfunction(L, l_settings_get);
    lua_setfield(L, -2, "get");
    lua_pushcfunction(L, l_settings_import);
    lua_setfield(L, -2, "import");
    lua_setglobal(L, "Settings");
}
