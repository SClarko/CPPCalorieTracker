@echo off
setlocal enabledelayedexpansion

REM Default build config
set CONFIG=Debug
if not "%1"=="" set CONFIG=%1

REM Create build folder if missing
if not exist build (
    mkdir build
)

REM Configure + build
cd build

cmake .. 
if errorlevel 1 (
    echo.
    echo CMake configure failed.
    pause
    exit /b 1
)

cmake --build . --config %CONFIG%
if errorlevel 1 (
    echo.
    echo Build failed.
    pause
    exit /b 1
)

REM Run (from build folder so ../data/... works)
echo.
echo Running CPPCalorieTracker (%CONFIG%)...
echo.

"%CONFIG%\CPPCalorieTracker.exe"

echo.
pause
endlocal