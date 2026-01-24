@echo off
setlocal enabledelayedexpansion

if "%1"=="" (
    echo Error: Build type not specified
    echo Usage: %0 [Debug^|Release]
    exit /b 1
)

set BUILD_TYPE=%1

if /i not "%BUILD_TYPE%"=="Debug" if /i not "%BUILD_TYPE%"=="Release" (
    echo Error: Invalid build type '%BUILD_TYPE%'. Valid values: Debug, Release
    exit /b 1
)

echo Setting up project environment for %BUILD_TYPE% build type...

if not exist "%USERPROFILE%\.conan2\profiles\default" (
    echo Creating default Conan profile...
    conan profile detect --force
)

echo Installing Conan dependencies...
conan install . --build=missing -pr profiles/%BUILD_TYPE%

echo Environment setup completed successfully!
