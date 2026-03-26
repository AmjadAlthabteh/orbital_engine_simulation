@echo off
title Black Hole Simulator - Icon Chooser
color 0A

echo.
echo ========================================
echo   CHOOSE YOUR APP ICON
echo ========================================
echo.
echo Available Icons:
echo.
echo   [1] Black Hole (Orange/Black)
echo       - Black event horizon
echo       - Orange accretion disk
echo       - Gravitational lensing
echo.
echo   [2] Saturn (Golden/Tan)
echo       - Beautiful rings
echo       - Golden planet
echo       - Classic space theme
echo.
echo   [3] Keep current icon
echo.
echo ========================================

set /p choice="Enter your choice (1-3): "

if "%choice%"=="1" (
    echo.
    echo Selecting Black Hole icon...
    if exist "black_hole_icon.ico" (
        echo [OK] Black Hole icon already active!
    ) else (
        echo [ERROR] black_hole_icon.ico not found!
        echo Run: python create_icon.py
    )
    goto done
)

if "%choice%"=="2" (
    echo.
    echo Selecting Saturn icon...
    if exist "saturn_icon.ico" (
        copy /Y "saturn_icon.ico" "black_hole_icon.ico" >nul
        echo [OK] Saturn icon activated!
        echo       Copied to: black_hole_icon.ico
    ) else (
        echo [ERROR] saturn_icon.ico not found!
        echo Run: python create_saturn_icon.py
        goto end
    )
    goto done
)

if "%choice%"=="3" (
    echo.
    echo Keeping current icon.
    goto end
)

echo.
echo Invalid choice! Please run again and select 1-3.
goto end

:done
echo.
echo ========================================
echo   NEXT STEPS
echo ========================================
echo.
echo 1. Open Visual Studio
echo 2. Build -^> Rebuild Solution
echo 3. Run your simulator
echo.
echo Your icon will appear in:
echo   - Window title bar
echo   - Taskbar
echo   - .exe file
echo.

:end
pause
