@echo off
setlocal
REM =========================
REM Build using g++ / MinGW
REM =========================

cd /d "%~dp0"
cd ..

cmake -B Build/GCC -S . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DUSE_ENGINE_SUBMODULE=OFF -D CMAKE_PREFIX_PATH="D:\Dev\Projects\Termynth\Build\GCC\Install"
cmake --build Build/GCC  --target Sandbox

echo.
echo Build with g++ completed.
pause