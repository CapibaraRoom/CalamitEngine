# Calamit-Engine

> Игровой движок для [Clamitasia](https://vk.ru/clamitasia)

Calamit - основа для разработки gui-приложений и игр на основе библиотек для отрисовки графии ([RayLib](https://github.com/raysan5/raylib)) и вспомогательным языком для скриптинга ([Lua](https://www.lua.org)). Основной идеей является использование низкоуровневых средств работы с графикой без излишних надстроек ради сохранения скорости. Для работы с ними используется упрощенная архитектура построения программ, вдохновленная [Godot](https://godotengine.org/).

## Сборка

```
git clone https://github.com/CapibaraRoom/CalamitEngine.git
cd CalamitEngine

chmod +x ./getlibs.sh && ./getlibs.sh

xmake clean && xmake
```

## Используемые средства:
> - Engine: [RayLib](https://github.com/raysan5/raylib)
> - Scripts: [Lua](https://www.lua.org)+[Sol2](https://github.com/ThePhD/sol2); [LuaJIT](https://github.com/LuaJIT/LuaJIT)
> - LowPoly-3D: [Blockbench](https://blockbench.ru/latest/appimage)
> - Reference: [Godot](https://godotengine.org/)
