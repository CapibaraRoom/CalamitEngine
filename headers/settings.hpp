#ifndef CALAMIT_SETTINGS_HPP
#define CALAMIT_SETTINGS_HPP

#include <string>
#include <vector>
#include <functional>
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

    void loadFromLua(const std::string& path) {
        sol::state lua;
        lua.open_libraries(sol::lib::base, sol::lib::package);
        try {
            sol::table config = lua.script_file(path).get<sol::table>();
            for (auto& [key, setter] : mappings)
                if (config[key].valid())
                    setter(config[key]);
            std::cout << "lua-settings was loaded: " << path << "\n";
        } catch (const sol::error& e) {
            std::cerr << "ERROR: lua-settings: " << e.what() << "\n";
        }
    }

private:
    std::vector<std::pair<std::string, std::function<void(sol::object)>>> mappings;

    template <typename U>
    void addMapping(const std::string& key, Setting<U>& field) {
        mappings.push_back({key, [&field](sol::object obj) {
            field.setFromLua(obj);
        }});
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
