@echo off
setlocal
REM =========================
REM Clean Build folder
REM =========================

cd /d "%~dp0"
cd ..

echo Removing build folders...
rmdir /s /q Build

echo Done!
pause