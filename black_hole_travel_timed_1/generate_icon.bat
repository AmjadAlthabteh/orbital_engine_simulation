@echo off
title Icon Generator - Black Hole Simulator
color 0B
echo.
echo ========================================
echo   ICON GENERATOR
echo ========================================
echo.
echo Choose which icons to generate:
echo.
echo   [1] Black Hole icon only
echo   [2] Saturn icon only
echo   [3] Both icons (Recommended)
echo.
set /p iconchoice="Your choice (1-3): "

echo.
echo ========================================

REM Check if Python is installed
python --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: Python is not installed!
    echo Please install Python from https://www.python.org/
    echo.
    pause
    exit /b 1
)

echo [1/4] Checking for Pillow library...
python -c "import PIL" >nul 2>&1
if errorlevel 1 (
    echo Pillow not found. Installing...
    pip install pillow
    if errorlevel 1 (
        echo ERROR: Failed to install Pillow
        echo Try manually: pip install pillow
        pause
        exit /b 1
    )
) else (
    echo   - Pillow is already installed
)

echo.
echo [2/4] Generating icons...

if "%iconchoice%"=="1" (
    python create_icon.py
    if errorlevel 1 goto error
    echo   [OK] Black Hole icon generated
)

if "%iconchoice%"=="2" (
    python create_saturn_icon.py
    if errorlevel 1 goto error
    echo   [OK] Saturn icon generated
    copy /Y "saturn_icon.ico" "black_hole_icon.ico" >nul
    echo   [OK] Set as active icon
)

if "%iconchoice%"=="3" (
    python create_icon.py
    if errorlevel 1 goto error
    echo   [OK] Black Hole icon generated

    python create_saturn_icon.py
    if errorlevel 1 goto error
    echo   [OK] Saturn icon generated
)

echo.
echo [3/4] Verifying icon files...
if exist "black_hole_icon.ico" (
    echo   [OK] Primary icon ready: black_hole_icon.ico
) else (
    echo ERROR: Icon file not found!
    goto error
)

if "%iconchoice%"=="3" (
    if exist "saturn_icon.ico" (
        echo   [OK] Alternate icon ready: saturn_icon.ico
    )
)

echo.
echo [4/4] Icon selection...
if "%iconchoice%"=="3" (
    echo.
    echo You now have BOTH icons!
    echo Run "choose_icon.bat" anytime to switch between them.
    echo.
    echo Currently active: Black Hole icon
)

echo.
echo ========================================
echo SUCCESS! Icon is ready!
echo ========================================
echo.
echo Next steps:
echo 1. Open Visual Studio
echo 2. Build -^> Rebuild Solution
echo 3. Run your Black Hole Travel Simulator
echo.
echo Your app will now have a cool space icon!
echo.
goto end

:error
echo.
echo ERROR: Failed to generate icon
echo Check that Pillow is installed: pip install pillow
echo.

:end
pause
