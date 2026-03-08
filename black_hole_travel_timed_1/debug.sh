#!/bin/bash
# Debug script with GDB/LLDB

set -e

BUILD_TYPE="Debug"
DEBUGGER="gdb"
SANITIZER=""

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --gdb)
            DEBUGGER="gdb"
            shift
            ;;
        --lldb)
            DEBUGGER="lldb"
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
        --valgrind)
            DEBUGGER="valgrind"
            shift
            ;;
        --help|-h)
            echo "Usage: ./debug.sh [options]"
            echo ""
            echo "Options:"
            echo "  --gdb           Use GDB debugger (default)"
            echo "  --lldb          Use LLDB debugger"
            echo "  --valgrind      Use Valgrind for memory checking"
            echo "  --asan          Enable Address Sanitizer"
            echo "  --ubsan         Enable Undefined Behavior Sanitizer"
            echo "  --help, -h      Show this help message"
            echo ""
            echo "GDB Commands:"
            echo "  run             Start the program"
            echo "  break main      Set breakpoint at main"
            echo "  break file.cpp:123  Set breakpoint at line 123"
            echo "  next (n)        Step over"
            echo "  step (s)        Step into"
            echo "  continue (c)    Continue execution"
            echo "  backtrace (bt)  Show call stack"
            echo "  print var       Print variable value"
            echo "  info locals     Show local variables"
            echo "  quit            Exit debugger"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Build first
echo "Building with debug symbols..."
./build.sh --debug --gcc $( [ -n "$SANITIZER" ] && echo "--$SANITIZER" )

cd build

# Run with debugger
echo ""
echo "Starting debugger: $DEBUGGER"
echo ""

case $DEBUGGER in
    gdb)
        echo "GDB Quick Reference:"
        echo "  run (r)         - Start program"
        echo "  break main      - Set breakpoint"
        echo "  next (n)        - Step over"
        echo "  step (s)        - Step into"
        echo "  continue (c)    - Continue"
        echo "  backtrace (bt)  - Call stack"
        echo "  print <var>     - Print variable"
        echo "  quit (q)        - Exit"
        echo ""
        gdb ./BlackHoleTravelSimulator
        ;;
    lldb)
        echo "LLDB Quick Reference:"
        echo "  run (r)         - Start program"
        echo "  breakpoint set --name main"
        echo "  next (n)        - Step over"
        echo "  step (s)        - Step into"
        echo "  continue (c)    - Continue"
        echo "  thread backtrace - Call stack"
        echo "  print <var>     - Print variable"
        echo "  quit (q)        - Exit"
        echo ""
        lldb ./BlackHoleTravelSimulator
        ;;
    valgrind)
        echo "Running Valgrind memory checker..."
        echo ""
        valgrind \
            --leak-check=full \
            --show-leak-kinds=all \
            --track-origins=yes \
            --verbose \
            --log-file=valgrind-output.txt \
            ./BlackHoleTravelSimulator

        echo ""
        echo "Valgrind output saved to: valgrind-output.txt"
        ;;
esac
