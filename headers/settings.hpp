#ifndef CALAMIT_SETTINGS_HPP
#define CALAMIT_SETTINGS_HPP

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <raylib.h>
#include <sol/sol.hpp>
#include <iostream>
#include "luaConverter.hpp"

namespace calamit {

template <typename T>
class Setting {
public:
    Setting() = default;
    Setting(T val) : value(val) {}

    T get() const { return value; }
    void set(const T& val) { value = val; }
    void set(T&& val) { value = std::move(val); }

    void setFromLua(sol::object obj) {
        if (obj.valid())
            value = LuaConverter<T>::convert(obj);
    }

private:
    T value{};
};

class Settings {
public:
    Settings() { initMappings(); }

    Setting<int> target_fps{60};
    Setting<std::string> project_name{"CalamitProject"};
    Setting<Vector2> window_size{{800.0f, 600.0f}};
    Setting<bool> resizable{false};

    void setValue(const std::string& key, sol::object value) {
        auto it = mappings_map.find(key);
        if (it != mappings_map.end())
            it->second(value);
        else
            std::cerr << "Warning: unknown setting key: " << key << "\n";
    }

    void loadFromLua(const std::string& path) {
        sol::state lua;
        lua.open_libraries(sol::lib::base, sol::lib::package);

        lua.set_function("set_setting", [this](const std::string& key, sol::object value) {
            this->setValue(key, value);
        });

        try {
            lua.script_file(path);
            std::cout << "Lua settings script executed: " << path << "\n";
        } catch (const sol::error& e) {
            std::cerr << "ERROR: lua-settings: " << e.what() << "\n";
        }
    }

private:
    std::unordered_map<std::string, std::function<void(sol::object)>> mappings_map;
    template <typename U>
    void addMapping(const std::string& key, Setting<U>& field) {
        mappings_map[key] = [&field](sol::object obj) {
            field.setFromLua(obj);
        };
    }

    void initMappings() {
        addMapping("target_fps", target_fps);
        addMapping("project_name", project_name);
        addMapping("window_size", window_size);
        addMapping("resizable", resizable);
    }
};

}

#endif
