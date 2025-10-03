@echo off
setlocal
REM =========================
REM Build using #Compiler#
REM =========================

cd /d "%~dp0"
cd ..

cmake -B Build/#Compiler# -S . -G "#Generator#" -DCMAKE_BUILD_TYPE=#Configuration# -DUSE_ENGINE_SUBMODULE="#Use_Submodule#" [[-D CMAKE_PREFIX_PATH="#Engine_Install_Location#"]]
cmake --build Build/#Compiler#  --target #Template_Blank##Target#

echo.
echo Build with #Compiler# completed.
pause
