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



class EngineSettings {
private:
    std::unordered_map<std::string, std::function<void(lua_State*, int)>> setters;
    std::unordered_map<std::string, std::function<void(lua_State*)>> getters;

public:
    template<typename T>
    void add(const char* name, T* var) {
        setters[name] = [var](lua_State* stack, int index) {
            if constexpr (std::is_same_v<T, int>)
                *var = static_cast<T>(lua_tointeger(stack, index));
            else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
                *var = static_cast<T>(lua_tonumber(stack, index));
            else if constexpr (std::is_same_v<T, bool>)
                *var = lua_toboolean(stack, index) != 0;
            else if constexpr (std::is_same_v<T, std::string>)
                *var = luaL_optstring(stack, index, "");
            else static_assert(sizeof(T) == 0, "Unsupported type");
        };

        getters[name] = [var](lua_State* stack) {
            if constexpr (std::is_same_v<T, int>)
                lua_pushinteger(stack, *var);
            else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>)
                lua_pushnumber(stack, *var);
            else if constexpr (std::is_same_v<T, bool>)
                lua_pushboolean(stack, *var);
            else if constexpr (std::is_same_v<T, std::string>)
                lua_pushstring(stack, var->c_str());
            else static_assert(sizeof(T) == 0, "Unsupported type");
        };
    }

    bool set(const std::string& name, lua_State* stack, int index) {
        auto element = setters.find(name);
        if (element == setters.end()) return false;
        element->second(stack, index);
        return true;
    }

    bool get(const std::string& name, lua_State* stack) {
        auto element = getters.find(name);
        if (element == getters.end()) return false;
        element->second(stack);
        return true;
    }
};



inline EngineSettings engine_settings;



#define DEFINE_SETTING(type, name, default_value) \
type name = default_value; \
namespace { \
    struct SettingReg_##name { \
        SettingReg_##name() { engine_settings.add(#name, &name); } \
    } setting_reg_##name; \
}



static int lua_settings_set(lua_State* stack) {
    const char* name = luaL_checkstring(stack, 1);
    if (engine_settings.set(name, stack, 2)) return 0;
    return luaL_error(stack, "Settings.hpp: lua_settings_set (unknown value): %s\n", name);
}

static int lua_settings_get(lua_State* stack) {
    const char* name = luaL_checkstring(stack, 1);
    if (engine_settings.get(name, stack)) return 1;
    // return luaL_error(stack, "Settings.hpp: lua_settings_get (unknown value): %s\n", name);
    lua_pushnil(stack);
    return 1;
}

static int lua_settings_import(lua_State* stack) {
    const char* path = luaL_checkstring(stack, 1);
    if (luaL_dofile(stack, path) == LUA_OK)
        printf("Settings loaded: %s\n", path);
    else {
        fprintf(stderr, "Settings.hpp: a_settings_import (error import): %s\n", lua_tostring(stack, -1));
        lua_pop(stack, 1);
    }
    lua_settop(stack, 0);
    return 0;
}

inline void register_settings_table(lua_State* stack) {
    lua_newtable(stack);

    lua_pushcfunction(stack, lua_settings_set);
    lua_setfield(stack, -2, "set");

    lua_pushcfunction(stack, lua_settings_get);
    lua_setfield(stack, -2, "get");

    lua_pushcfunction(stack, lua_settings_import);
    lua_setfield(stack, -2, "import");

    lua_setglobal(stack, "Settings");
}
