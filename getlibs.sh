#!/bin/sh

rm -rf libs/raylib libs/luajit libs/sol2

mkdir -p libs/_download
mkdir -p libs/raylib
mkdir -p libs/luajit/include
mkdir -p libs/luajit/lib
mkdir -p libs/sol2/include

cd ./libs/_download

# -------------------- Raylib 6.0 --------------------
echo "Скачиваем Raylib 6.0..."
wget -nc https://github.com/raysan5/raylib/releases/download/6.0/raylib-6.0_linux_amd64.tar.gz
tar -xzf raylib-6.0_linux_amd64.tar.gz -C ../raylib --strip-components=1

# -------------------- Lua 5.4.8 --------------------
echo "Скачиваем Lua 5.4.8..."
wget -nc https://www.lua.org/ftp/lua-5.4.8.tar.gz
tar -xzf lua-5.4.8.tar.gz
cd lua-5.4.8
make linux -j$(nproc)
cp src/liblua.a ../../luajit/lib/
cp src/*.h ../../luajit/include/
cd ..
rm -rf lua-5.4.8

# -------------------- Sol2 v3.5.0 --------------------
echo "Скачиваем Sol2 v3.5.0..."
wget -nc https://github.com/ThePhD/sol2/archive/refs/tags/v3.5.0.tar.gz -O sol2-v3.5.0.tar.gz
tar -xzf sol2-v3.5.0.tar.gz
cp -r sol2-3.5.0/include/sol ../sol2/include/
rm -rf sol2-3.5.0

cd ..

echo "Готово! Все зависимости установлены"
