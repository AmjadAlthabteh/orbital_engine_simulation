# 🔧 Terminal Build & Debug Guide

Complete guide for building, testing, and debugging through the terminal using GCC, Clang, CMake, GDB, and LLVM tools.

---

## 📋 Prerequisites

### Install Required Tools

#### Windows (Git Bash / WSL / MSYS2):
```bash
# Using MSYS2
pacman -S mingw-w64-x86_64-gcc
pacman -S mingw-w64-x86_64-clang
pacman -S mingw-w64-x86_64-cmake
pacman -S mingw-w64-x86_64-gdb

# Or use WSL (Ubuntu):
sudo apt update
sudo apt install build-essential cmake gdb clang lldb valgrind
```

#### Linux (Ubuntu/Debian):
```bash
sudo apt update
sudo apt install build-essential cmake gdb clang lldb valgrind cppcheck clang-tidy
```

#### macOS:
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Install Homebrew (if not installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install tools
brew install cmake lldb cppcheck
```

---

## 🔨 Building

### Quick Start

#### Option 1: Using Build Script (Recommended)
```bash
# Make script executable (first time only)
chmod +x build.sh

# Debug build with GCC
./build.sh --debug --gcc

# Release build with Clang
./build.sh --release --clang

# Build and run
./build.sh --debug --gcc --run

# Clean build
./build.sh --clean --debug --gcc
```

#### Option 2: Manual CMake
```bash
# Create build directory
mkdir build && cd build

# Configure (Debug)
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build . -j8

# Run
./BlackHoleTravelSimulator
```

### Build Options

```bash
./build.sh [options]

Options:
  --debug         Build in Debug mode (default)
  --release       Build in Release mode
  --gcc           Use GCC compiler (default)
  --clang         Use Clang compiler
  --clean         Clean build directory first
  --run           Run after building
  --asan          Enable Address Sanitizer
  --ubsan         Enable Undefined Behavior Sanitizer
  --tsan          Enable Thread Sanitizer
  --help, -h      Show help message
```

### Examples

```bash
# Debug build with GCC
./build.sh --debug --gcc

# Release build with optimizations
./build.sh --release --clang

# Debug with Address Sanitizer (detect memory bugs!)
./build.sh --debug --asan --run

# Clean release build
./build.sh --clean --release --gcc
```

---

## 🐛 Debugging

### Using GDB (GNU Debugger)

#### Quick Start
```bash
# Make debug script executable
chmod +x debug.sh

# Start GDB
./debug.sh --gdb
```

#### Manual GDB
```bash
# Build with debug symbols first
./build.sh --debug --gcc

# Start GDB
cd build
gdb ./BlackHoleTravelSimulator
```

#### Essential GDB Commands

```gdb
# Start the program
(gdb) run

# Set breakpoint at main
(gdb) break main

# Set breakpoint at specific line
(gdb) break main.cpp:500

# Set breakpoint in function
(gdb) break calculateTransfer

# Step over (next line)
(gdb) next
(gdb) n

# Step into (enter function)
(gdb) step
(gdb) s

# Continue execution
(gdb) continue
(gdb) c

# Print variable
(gdb) print ship.position
(gdb) p ship.velocity.x

# Show call stack
(gdb) backtrace
(gdb) bt

# Show call stack with frame numbers
(gdb) backtrace full

# Show local variables
(gdb) info locals

# Show all variables
(gdb) info args

# Watch variable (break when it changes)
(gdb) watch deltaV

# List source code
(gdb) list
(gdb) l

# Show current location
(gdb) where

# Exit debugger
(gdb) quit
```

#### Advanced GDB Usage

```bash
# Run with arguments
(gdb) run --argument value

# Attach to running process
gdb -p <pid>

# Conditional breakpoint
(gdb) break main.cpp:500 if ship.altitude < 5.0

# Display variable every step
(gdb) display ship.position

# Print array
(gdb) print bodies[0]->position

# Print structure
(gdb) print *ship

# Set variable value
(gdb) set var altitude = 10.0

# Save breakpoints
(gdb) save breakpoints breakpoints.gdb

# Load breakpoints
(gdb) source breakpoints.gdb
```

### Using LLDB (LLVM Debugger)

```bash
# Start LLDB
./debug.sh --lldb

# Or manually
cd build
lldb ./BlackHoleTravelSimulator
```

#### LLDB Commands

```lldb
# Start program
(lldb) run
(lldb) r

# Set breakpoint
(lldb) breakpoint set --name main
(lldb) b main

# Breakpoint at line
(lldb) breakpoint set --file main.cpp --line 500
(lldb) b main.cpp:500

# Step over
(lldb) next
(lldb) n

# Step into
(lldb) step
(lldb) s

# Continue
(lldb) continue
(lldb) c

# Print variable
(lldb) print ship.position
(lldb) p ship.velocity

# Call stack
(lldb) thread backtrace
(lldb) bt

# Show variables
(lldb) frame variable

# Watch variable
(lldb) watchpoint set variable deltaV

# Exit
(lldb) quit
```

---

## 🧪 Memory Checking & Sanitizers

### Address Sanitizer (ASan) - Detect Memory Bugs

```bash
# Build with ASan
./build.sh --debug --asan --run

# What it detects:
# - Use after free
# - Heap buffer overflow
# - Stack buffer overflow
# - Memory leaks
# - Use after return
```

**Example Output:**
```
=================================================================
==12345==ERROR: AddressSanitizer: heap-use-after-free on address 0x...
READ of size 4 at 0x... thread T0
    #0 0x... in calculateTransfer() main.cpp:234
    #1 0x... in main main.cpp:500
```

### Undefined Behavior Sanitizer (UBSan)

```bash
# Build with UBSan
./build.sh --debug --ubsan --run

# What it detects:
# - Integer overflow
# - Division by zero
# - Invalid pointer operations
# - Null pointer dereference
# - Array out of bounds
```

### Valgrind - Advanced Memory Analysis

```bash
# Run with Valgrind
./debug.sh --valgrind

# Or manually
cd build
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         --log-file=valgrind-output.txt \
         ./BlackHoleTravelSimulator
```

**Valgrind checks:**
- Memory leaks
- Invalid memory access
- Use of uninitialized values
- Invalid frees
- Overlapping memory operations

---

## 📊 Static Analysis

### Run All Analysis Tools

```bash
# Make script executable
chmod +x analyze.sh

# Run all analyzers
./analyze.sh --all

# Or run specific tool
./analyze.sh --cppcheck
./analyze.sh --clang-tidy
./analyze.sh --scan-build
```

### CPPCheck - Static Code Analyzer

```bash
cppcheck --enable=all \
         --suppress=missingIncludeSystem \
         --std=c++17 \
         --verbose \
         *.cpp *.hpp
```

### Clang-Tidy - Linter

```bash
# First generate compile_commands.json
cd build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cd ..

# Run clang-tidy
clang-tidy -p build *.cpp
```

### Scan-Build - Clang Static Analyzer

```bash
# Clean build with scan-build
rm -rf build
mkdir build && cd build
scan-build cmake ..
scan-build make
```

---

## 🔍 Call Stack Analysis

### Getting Call Stack During Crash

#### Method 1: Core Dump (Linux/macOS)

```bash
# Enable core dumps
ulimit -c unlimited

# Run program (will create core file on crash)
./BlackHoleTravelSimulator

# Analyze core dump
gdb ./BlackHoleTravelSimulator core

# In GDB, get stack trace
(gdb) backtrace
(gdb) thread apply all backtrace
```

#### Method 2: Run in Debugger

```bash
# Start in GDB
gdb ./BlackHoleTravelSimulator

# Run until crash
(gdb) run

# When it crashes, get stack
(gdb) backtrace
(gdb) backtrace full  # With local variables
```

#### Method 3: Catch Segfaults

Add to your code:
```cpp
#include <csignal>

void signalHandler(int signum) {
    std::cerr << "Caught signal " << signum << "\n";
    // Print stack trace here
    exit(signum);
}

int main() {
    signal(SIGSEGV, signalHandler);
    signal(SIGABRT, signalHandler);
    // ... rest of code
}
```

---

## 🧬 Profiling Performance

### Using Gprof

```bash
# Compile with profiling
g++ -pg -O2 *.cpp -o simulator

# Run program (creates gmon.out)
./simulator

# Generate profile report
gprof simulator gmon.out > profile.txt
```

### Using Perf (Linux)

```bash
# Record performance data
perf record -g ./BlackHoleTravelSimulator

# View report
perf report

# See hotspots
perf top
```

### Using Valgrind Callgrind

```bash
# Profile with callgrind
valgrind --tool=callgrind ./BlackHoleTravelSimulator

# Visualize with kcachegrind
kcachegrind callgrind.out.*
```

---

## 🎯 Common Issues & Solutions

### Issue: "Cannot find -lsfml-graphics"

**Solution:**
```bash
# Update CMakeLists.txt with correct SFML path
set(SFML_DIR "/path/to/your/SFML/lib/cmake/SFML")
```

### Issue: "glad.c not found"

**Solution:**
```bash
# Update CMakeLists.txt with correct GLAD path
set(GLAD_SOURCE "/path/to/glad/src/glad.c")
set(GLAD_INCLUDE_DIR "/path/to/glad/include")
```

### Issue: Segmentation Fault

**Debug with:**
```bash
# 1. Run with Address Sanitizer
./build.sh --debug --asan --run

# 2. Run in GDB
gdb ./BlackHoleTravelSimulator
(gdb) run
(gdb) backtrace

# 3. Check with Valgrind
valgrind ./BlackHoleTravelSimulator
```

### Issue: Slow Performance

**Profile with:**
```bash
# 1. Build in Release mode
./build.sh --release

# 2. Profile with perf
perf record -g ./BlackHoleTravelSimulator
perf report

# 3. Check compiler optimizations
g++ -O3 -march=native
```

---

## 📚 Cheat Sheet

### Quick Commands

```bash
# Build and run
./build.sh --debug --run

# Debug with GDB
./debug.sh --gdb

# Check for memory leaks
./build.sh --asan --run

# Analyze code
./analyze.sh --all

# Clean everything
rm -rf build *.o *.out core

# Rebuild from scratch
./build.sh --clean --debug --gcc
```

### GDB Quick Reference

```
r       - run
b       - breakpoint
n       - next (step over)
s       - step (step into)
c       - continue
p       - print
bt      - backtrace
l       - list source
q       - quit
```

---

## 🚀 Advanced Tips

### 1. Compile-Time Optimization Flags

```cmake
# In CMakeLists.txt
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto")
```

### 2. Debug Symbols in Release

```bash
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
```

### 3. Parallel Compilation

```bash
# Use all CPU cores
cmake --build . -j$(nproc)

# Specific number of cores
cmake --build . -j8
```

### 4. Verbose Build Output

```bash
cmake --build . --verbose
# Or
make VERBOSE=1
```

### 5. Cross-Compilation

```bash
# For ARM
cmake .. -DCMAKE_TOOLCHAIN_FILE=arm-toolchain.cmake
```

---

## 📖 Additional Resources

- **GDB Documentation**: https://sourceware.org/gdb/documentation/
- **LLDB Tutorial**: https://lldb.llvm.org/use/tutorial.html
- **Valgrind Manual**: https://valgrind.org/docs/manual/manual.html
- **CMake Documentation**: https://cmake.org/documentation/
- **AddressSanitizer**: https://github.com/google/sanitizers

---

**Happy Debugging!** 🐛🔧
