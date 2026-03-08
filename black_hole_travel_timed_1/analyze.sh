#!/bin/bash
# Static analysis and code checking tools

set -e

TOOL="all"

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --cppcheck)
            TOOL="cppcheck"
            shift
            ;;
        --clang-tidy)
            TOOL="clang-tidy"
            shift
            ;;
        --scan-build)
            TOOL="scan-build"
            shift
            ;;
        --all)
            TOOL="all"
            shift
            ;;
        --help|-h)
            echo "Usage: ./analyze.sh [options]"
            echo ""
            echo "Options:"
            echo "  --cppcheck      Run cppcheck static analyzer"
            echo "  --clang-tidy    Run clang-tidy linter"
            echo "  --scan-build    Run clang static analyzer"
            echo "  --all           Run all tools (default)"
            echo "  --help, -h      Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

echo "╔════════════════════════════════════════════╗"
echo "║  Black Hole Simulator - Code Analysis     ║"
echo "╚════════════════════════════════════════════╝"
echo ""

# CPPCheck
if [ "$TOOL" == "all" ] || [ "$TOOL" == "cppcheck" ]; then
    echo "Running CPPCheck..."
    if command -v cppcheck &> /dev/null; then
        cppcheck \
            --enable=all \
            --suppress=missingIncludeSystem \
            --suppress=unusedFunction \
            --inline-suppr \
            --std=c++17 \
            --verbose \
            --output-file=cppcheck-report.txt \
            *.cpp *.hpp 2>&1 | tee cppcheck-output.log

        echo "✓ CPPCheck complete. Report: cppcheck-report.txt"
        echo ""
    else
        echo "⚠ cppcheck not found. Install with: sudo apt install cppcheck"
        echo ""
    fi
fi

# Clang-Tidy
if [ "$TOOL" == "all" ] || [ "$TOOL" == "clang-tidy" ]; then
    echo "Running Clang-Tidy..."
    if command -v clang-tidy &> /dev/null; then
        # Build compile_commands.json first
        mkdir -p build
        cd build
        cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        cd ..

        clang-tidy \
            -p build \
            *.cpp \
            --checks='*,-fuchsia-*,-google-*,-llvm-header-guard' \
            > clang-tidy-report.txt 2>&1

        echo "✓ Clang-Tidy complete. Report: clang-tidy-report.txt"
        echo ""
    else
        echo "⚠ clang-tidy not found. Install with: sudo apt install clang-tidy"
        echo ""
    fi
fi

# Scan-Build (Clang Static Analyzer)
if [ "$TOOL" == "all" ] || [ "$TOOL" == "scan-build" ]; then
    echo "Running Clang Static Analyzer (scan-build)..."
    if command -v scan-build &> /dev/null; then
        rm -rf build-scan
        mkdir -p build-scan
        cd build-scan

        scan-build cmake ..
        scan-build -o ../scan-build-results make -j4

        cd ..
        echo "✓ Scan-build complete. Results in: scan-build-results/"
        echo ""
    else
        echo "⚠ scan-build not found. Install with: sudo apt install clang-tools"
        echo ""
    fi
fi

echo "════════════════════════════════════════════"
echo "Analysis complete!"
echo ""
echo "Reports generated:"
echo "  - cppcheck-report.txt"
echo "  - clang-tidy-report.txt"
echo "  - scan-build-results/"
echo ""
