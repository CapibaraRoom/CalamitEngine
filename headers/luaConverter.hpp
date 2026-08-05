#ifndef CALAMIT_LUA_CONVERTER_HPP
#define CALAMIT_LUA_CONVERTER_HPP

#include <sol/sol.hpp>
#include <raylib.h>

namespace calamit {

// ============================================================
//  Общий шаблон
// ============================================================
template <typename T>
struct LuaConverter {
    static T convert(sol::object obj) { return obj.as<T>(); }
};

// ============================================================
//  Специализация для Vector2 (Raylib)
// ============================================================
template <>
struct LuaConverter<Vector2> {
    static Vector2 convert(sol::object obj) {
        if (!obj.valid()) return Vector2{0, 0};

        if (obj.is<sol::table>()) {
            sol::table t = obj.as<sol::table>();

            // 1) массив [w, h]
            if (t.size() == 2)
                return Vector2{t[1].get<float>(), t[2].get<float>()};
                
            // 2) таблица с полями width / height
            else if (t["width"].valid() && t["height"].valid())
                return Vector2{t["width"].get<float>(), t["height"].get<float>()};
                
            // 3) таблица с полями x / y
            else if (t["x"].valid() && t["y"].valid())
                return Vector2{t["x"].get<float>(), t["y"].get<float>()};
        }

        // Если ничего не подошло – возвращаем (0,0)
        return Vector2{0, 0};
    }
};

}

#endif
