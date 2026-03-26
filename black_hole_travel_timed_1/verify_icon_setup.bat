@echo off
color 0A
echo.
echo ========================================
echo   ICON SETUP VERIFICATION
echo ========================================
echo.

echo Checking icon files...
echo.

if exist "black_hole_icon.ico" (
    echo [OK] black_hole_icon.ico exists
) else (
    echo [MISSING] black_hole_icon.ico
    echo Creating it now...
    python create_icon.py
    if exist "black_hole_icon.ico" (
        echo [OK] Icon created successfully!
    ) else (
        echo [ERROR] Failed to create icon
        goto error
    )
)

echo.
echo Checking resource files...
if exist "app_icon.rc" (
    echo [OK] app_icon.rc exists
) else (
    echo [ERROR] app_icon.rc missing!
    goto error
)

if exist "resource.h" (
    echo [OK] resource.h exists
) else (
    echo [ERROR] resource.h missing!
    goto error
)

echo.
echo Checking project file...
findstr /C:"app_icon.rc" "black_hole_travel_timed_1.vcxproj" >nul
if errorlevel 1 (
    echo [ERROR] Icon not configured in project!
    goto error
) else (
    echo [OK] Icon configured in project
)

echo.
echo ========================================
echo   ALL CHECKS PASSED!
echo ========================================
echo.
echo Your icon is ready to use!
echo.
echo What happens now:
echo  - Every time you BUILD in Visual Studio
echo  - The icon is AUTOMATICALLY included
echo  - It shows in window, taskbar, .exe file
echo.
echo Just build your project normally!
echo No extra steps needed!
echo.
goto end

:error
echo.
echo ========================================
echo   SETUP INCOMPLETE
echo ========================================
echo.
echo Please run: generate_icon.bat
echo Then rebuild your project.
echo.

:end
pause
