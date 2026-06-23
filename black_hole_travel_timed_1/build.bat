@echo off
setlocal

set BUILD_TYPE=Debug
set COMPILER=msvc
set CLEAN=0
set RUN=0
set SANITIZER=

:parse_args
if "%~1"=="" goto :end_parse
if /i "%~1"=="--release" set BUILD_TYPE=Release
if /i "%~1"=="--debug" set BUILD_TYPE=Debug
if /i "%~1"=="--msvc" set COMPILER=msvc
if /i "%~1"=="--mingw" set COMPILER=mingw
if /i "%~1"=="--clean" set CLEAN=1
if /i "%~1"=="--run" set RUN=1
if /i "%~1"=="--asan" set SANITIZER=asan
if /i "%~1"=="--help" goto :show_help
shift
goto :parse_args

:end_parse
echo ============================================
echo   Black Hole Simulator - Build Script
echo ============================================
echo.
echo Build type: %BUILD_TYPE%
echo Compiler:   %COMPILER%
if defined SANITIZER echo Sanitizer:  %SANITIZER%
echo.

if %CLEAN%==1 (
    echo Cleaning build directory...
    rmdir /s /q build 2>nul
)

if not exist build mkdir build
cd build

set CMAKE_ARGS=-DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if "%SANITIZER%"=="asan" set CMAKE_ARGS=%CMAKE_ARGS% -DENABLE_ASAN=ON

echo Configuring with CMake...
if "%COMPILER%"=="mingw" (
    cmake .. %CMAKE_ARGS% -G "MinGW Makefiles"
) else (
    cmake .. %CMAKE_ARGS%
)

if errorlevel 1 (
    echo ERROR: CMake configuration failed.
    exit /b 1
)

echo Building...
cmake --build . --config %BUILD_TYPE% -j %NUMBER_OF_PROCESSORS%
if errorlevel 1 (
    echo ERROR: Build failed.
    exit /b 1
)

echo.
echo Build successful.
echo.

if %RUN%==1 (
    echo Running Black Hole Simulator...
    %BUILD_TYPE%\BlackHoleTravelSimulator.exe
) else (
    echo Run with: cd build\%BUILD_TYPE% ^&^& BlackHoleTravelSimulator.exe
)

cd ..
exit /b 0

:show_help
echo Usage: build.bat [options]
echo.
echo Options:
echo   --debug         Build in Debug mode (default)
echo   --release       Build in Release mode
echo   --msvc          Use MSVC compiler (default)
echo   --mingw         Use MinGW compiler
echo   --clean         Clean build directory first
echo   --run           Run after building
echo   --asan          Enable Address Sanitizer
echo   --help          Show this help message
echo.
echo Examples:
echo   build.bat --debug --msvc
echo   build.bat --release --run
echo   build.bat --debug --asan --run
exit /b 0
