@echo off
setlocal
REM =========================
REM Build using g++ / MinGW
REM =========================

cd /d "%~dp0"
cd ..

if not exist BuildGCC mkdir BuildGCC
cd BuildGCC

cmake -B Build/GCC -S . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Shipping 
cmake --build Build/GCC  --target TermynthEngine

echo.
echo Build with g++ completed.
pause