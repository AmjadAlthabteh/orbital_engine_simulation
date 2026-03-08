#!/bin/bash
# Build script for GCC/Clang on Linux/macOS/Windows (Git Bash/WSL)

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Default values
BUILD_TYPE="Debug"
COMPILER="gcc"
CLEAN=false
RUN=false
SANITIZER=""

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --gcc)
            COMPILER="gcc"
            shift
            ;;
        --clang)
            COMPILER="clang"
            shift
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --run)
            RUN=true
            shift
            ;;
        --asan)
            SANITIZER="asan"
            shift
            ;;
        --ubsan)
            SANITIZER="ubsan"
            shift
            ;;
        --tsan)
            SANITIZER="tsan"
            shift
            ;;
        --help|-h)
            echo "Usage: ./build.sh [options]"
            echo ""
            echo "Options:"
            echo "  --debug         Build in Debug mode (default)"
            echo "  --release       Build in Release mode"
            echo "  --gcc           Use GCC compiler (default)"
            echo "  --clang         Use Clang compiler"
            echo "  --clean         Clean build directory first"
            echo "  --run           Run after building"
            echo "  --asan          Enable Address Sanitizer"
            echo "  --ubsan         Enable Undefined Behavior Sanitizer"
            echo "  --tsan          Enable Thread Sanitizer"
            echo "  --help, -h      Show this help message"
            echo ""
            echo "Examples:"
            echo "  ./build.sh --debug --gcc"
            echo "  ./build.sh --release --clang --run"
            echo "  ./build.sh --debug --asan --run"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

# Print configuration
echo -e "${BLUE}╔════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║  Black Hole Simulator - Build Script      ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════════╝${NC}"
echo ""
echo -e "${YELLOW}Configuration:${NC}"
echo -e "  Build Type: ${GREEN}${BUILD_TYPE}${NC}"
echo -e "  Compiler:   ${GREEN}${COMPILER}${NC}"
if [ -n "$SANITIZER" ]; then
    echo -e "  Sanitizer:  ${GREEN}${SANITIZER}${NC}"
fi
echo ""

# Set compiler
if [ "$COMPILER" == "clang" ]; then
    export CC=clang
    export CXX=clang++
else
    export CC=gcc
    export CXX=g++
fi

# Clean if requested
if [ "$CLEAN" == true ]; then
    echo -e "${YELLOW}Cleaning build directory...${NC}"
    rm -rf build
fi

# Create build directory
mkdir -p build
cd build

# CMake configuration
CMAKE_ARGS="-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"

if [ "$SANITIZER" == "asan" ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DENABLE_ASAN=ON"
elif [ "$SANITIZER" == "ubsan" ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DENABLE_UBSAN=ON"
elif [ "$SANITIZER" == "tsan" ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DENABLE_TSAN=ON"
fi

# Configure
echo -e "${YELLOW}Configuring with CMake...${NC}"
cmake .. $CMAKE_ARGS

# Build
echo -e "${YELLOW}Building...${NC}"
cmake --build . --config $BUILD_TYPE -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Check if build succeeded
if [ $? -eq 0 ]; then
    echo ""
    echo -e "${GREEN}✓ Build successful!${NC}"
    echo ""

    # Run if requested
    if [ "$RUN" == true ]; then
        echo -e "${YELLOW}Running Black Hole Simulator...${NC}"
        echo ""
        ./BlackHoleTravelSimulator
    else
        echo -e "Run with: ${BLUE}cd build && ./BlackHoleTravelSimulator${NC}"
    fi
else
    echo -e "${RED}✗ Build failed!${NC}"
    exit 1
fi
