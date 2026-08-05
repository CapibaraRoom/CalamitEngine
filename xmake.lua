set_project("testprog")
set_version("0.0.0")
set_languages("c++17")

target("Calamit")
    set_kind("binary")
    add_files("main.cpp")

    -- Raylib
    add_includedirs("libs/raylib/include")
    add_linkdirs("libs/raylib/lib")
    add_links("libraylib.a")

    -- Lua
    add_includedirs("libs/luajit/include")
    add_linkdirs("libs/luajit/lib")
    add_links("liblua.a")

    -- Sol2
    add_includedirs("libs/sol2/include")

    -- Системные зависимости для Linux
    if is_plat("linux") then
        add_links("pthread", "dl", "m")
        add_syslinks("X11")
    end

    -- Куда класть готовый бинарник
    set_targetdir("../CalamitEngine")

    -- Автозапуск после сборки
    after_build(function(target)
        local dir = target:targetdir()
        local lua_dst = path.join(dir, "lua")
        os.tryrm(lua_dst)
        os.cp("lua", lua_dst)
        os.cd(dir)
        os.execv(target:targetfile(), {})
    end)
