@echo off

@REM build raylib if not already built
IF NOT EXIST libs\raylib\src\libraylib.a (
    pushd libs\raylib\src
    make PLATFORM=PLATFORM_DESKTOP RAYLIB_LIBTYPE=STATIC
    popd
)

IF NOT EXIST build (
    mkdir build
)

@REM try different possible compilers
set COMPILERS=cc gcc clang

for %%C in (%COMPILERS%) do (
    %%C -o build\main.exe src\main.c -I libs\raylib\src -L libs\raylib\src -lraylib -lgdi32 -lwinmm
    if ERRORLEVEL 1 (
        echo Compilation failed with %%C
    ) else (
        echo Compilation succeeded with %%C
        exit /b 0
    )
)