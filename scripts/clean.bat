@echo off

echo Cleaning workspace...

if exist "build" (
    rmdir /s /q "build" || (
        echo Error: Failed to remove build directory
        exit /b 1
    )
)

if exist "CMakeUserPresets.json" (
    del "CMakeUserPresets.json" || (
        echo Error: Failed to remove CMakeUserPresets.json file
        exit /b 1
    )
)

echo Cleaning completed!
