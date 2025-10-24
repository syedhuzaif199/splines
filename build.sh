#!/bin/sh

# Build script for the splines project

mkdir -p build

# Build raylib if not already built
if [ ! -f libs/raylib/src/libraylib.a ]; then
    cd libs/raylib/src
    make PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=STATIC
    cd ../../..
fi

# Try different compilers

for CC in cc gcc clang; do
    $CC -o build/main src/main.c -I libs/raylib/src -L libs/raylib/src -lraylib -ldl -lm
    if [ $? -eq 0 ]; then
        echo "Compilation succeeded with $CC"
        break
    else
        echo "Compilation failed with $CC"
    fi
done