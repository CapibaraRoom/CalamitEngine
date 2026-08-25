# Calamit-Engine

> Игровой движок для [Clamitasia](https://vk.ru/clamitasia)

**Calamit** - основа для разработки gui-приложений и 2D/3D-программ на основе библиотек для отрисовки графики ([RayLib](https://github.com/raysan5/raylib)) и вспомогательного языка для скриптинга ([Lua](https://www.lua.org)). Основной идеей является использование низкоуровневых средств работы с графикой без излишних надстроек ради сохранения скорости. Для работы с ними используется упрощенная архитектура построения программ, вдохновленная [Godot](https://godotengine.org/).

**Важно!** Проект не использует системные API (WinAPI, GTK, Qt). Данный проект не подойдет для тесной интеграции в систему Windows.


## Сборка

```
git clone https://github.com/CapibaraRoom/CalamitEngine.git
cd CalamitEngine
./getlibs.sh
./take
```


## Очень нада
> базовые ноды gui  
> можно ли сделать импорт ассетов из библиотек [Unity](https://assetstore.unity.com/top-assets/top-free), [Godot](https://godotengine.org/asset-library/asset)?  
> ~~строениедерева нод~~  - надеюсь, что не придётся часто сюда влезать  
> ~~базовые функции lua~~  - танцевал я этот ваш luajit  
> ~~чтение настроек~~  - наконец, больше не нужно по сто раз всё перекомпилировать  
> ~~lua -> luajit~~  - ох и пожалею я об этом...  


## Используемые средства:
> - Engine: [RayLib](https://github.com/raysan5/raylib)
> - Scripts: [Lua](https://www.lua.org)+[Sol2](https://github.com/ThePhD/sol2); [LuaJIT](https://github.com/LuaJIT/LuaJIT)
> - LowPoly-3D: [Blockbench](https://blockbench.ru/latest/appimage)
> - Reference: [Godot](https://godotengine.org/)
