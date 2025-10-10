@echo off
setlocal
REM =========================
REM Build Visual Studio project files
REM =========================

cd /d "%~dp0"
cd ..

cmake -B Build/VS -S . -G "Visual Studio 17 2022"

echo.
echo Visual Studio solution generated
pause