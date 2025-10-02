@echo off
setlocal
REM =========================
REM Build Visual Studio project files
REM =========================

cd /d "%~dp0"
cd ..

if exist BuildVS (
    rmdir /s /q BuildVS
)
mkdir BuildVS
cd BuildVS

cmake .. -G "Visual Studio 17 2022"

echo.
echo Visual Studio solution generated
pause