@echo off
setlocal
REM =========================
REM Build using g++ / MinGW
REM =========================

cd /d "%~dp0"
cd ..

if not exist BuildGCC mkdir BuildGCC
cd BuildGCC

cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Shipping
mingw32-make Server

echo.
echo Build with g++ completed.
pause