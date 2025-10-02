@echo off
setlocal
REM =========================
REM Setup
REM =========================

set COMPILER=%1
set CONFIGURATION=%2
if "%CONFIGURATION%"=="" set CONFIGURATION=Shipping

if "%COMPILER%"=="" set COMPILER=GCC

REM Map compiler to CMake generator
if "%COMPILER%"=="GCC" set CMAKE_GEN="MinGW Makefiles"
if "%COMPILER%"=="MSVC" set CMAKE_GEN="Visual Studio 17 2022"

set BUILD_DIR=Build\%COMPILER%
set INSTALL_DIR=Build\%COMPILER%\Install

set OUTPUT_DIR=%BUILD_DIR%\Binaries\Win64\%CONFIGURATION%

if exist "%OUTPUT_DIR%\libTermynthEngine.a" (
	echo.
	echo Build files found in %OUTPUT_DIR%.
	goto :INSTALL
) else (
	echo.
	echo Build files NOT found. Compiling first...
	goto :COMPILE_SHIPPING
)

:COMPILE_SHIPPING
cmake -B %BUILD_DIR% -S . -G %CMAKE_GEN% -DCMAKE_BUILD_TYPE=%CONFIGURATION%
if errorLevel 1 goto :ERROR_EXIT

cmake --build %BUILD_DIR% --config %CONFIGURATION%
if errorLevel 1 goto :ERROR_EXIT

:INSTALL
echo.
echo Installing %CONFIGURATION% build to %INSTALL_DIR%...
cmake --install %BUILD_DIR% --prefix %INSTALL_DIR%
if errorLevel 1 goto :ERROR_EXIT

echo.
echo Installation successful!
goto :END

:ERROR_EXIT
echo.
echo !!! ERROR: A previous command failed. Installation aborted.
goto :END

:END
pause
endlocal