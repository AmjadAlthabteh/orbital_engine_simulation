# 🚀 Quick Start - Terminal Building

Ultra-quick guide to get you building and testing immediately!

---

## ⚡ 30-Second Setup

### Windows (Git Bash / PowerShell)

```bash
# 1. Make scripts executable
chmod +x build.sh debug.sh

# 2. Build and run
./build.sh --debug --run
```

### Linux / macOS

```bash
# 1. Make scripts executable
chmod +x build.sh debug.sh analyze.sh

# 2. Build and run
./build.sh --debug --run
```

---

## 🔨 Building

### Simple Build
```bash
./build.sh --debug --run
```

### Release Build (Fast!)
```bash
./build.sh --release --run
```

### With Different Compilers
```bash
# GCC
./build.sh --debug --gcc --run

# Clang/LLVM
./build.sh --debug --clang --run
```

---

## 🐛 Debugging

### Quick Debug Session
```bash
./debug.sh --gdb
```

**In GDB:**
```gdb
(gdb) break main          # Set breakpoint
(gdb) run                 # Start program
(gdb) next                # Step over
(gdb) print ship.altitude # Print variable
(gdb) backtrace          # Show call stack
(gdb) quit               # Exit
```

### Find Memory Leaks
```bash
./build.sh --asan --run
```

**Output shows:**
- Memory leaks
- Buffer overflows
- Use-after-free bugs
- Exact line numbers!

---

## 🧪 Testing

### Check for Bugs
```bash
# Address Sanitizer (memory bugs)
./build.sh --asan --run

# Undefined Behavior Sanitizer
./build.sh --ubsan --run

# Valgrind (advanced memory check)
./debug.sh --valgrind
```

### Static Analysis
```bash
./analyze.sh --all
```

---

## 📊 Common Commands

| What You Want | Command |
|---------------|---------|
| **Build** | `./build.sh --debug` |
| **Build & Run** | `./build.sh --debug --run` |
| **Fast Build** | `./build.sh --release` |
| **Find Bugs** | `./build.sh --asan --run` |
| **Debug** | `./debug.sh --gdb` |
| **Clean Build** | `./build.sh --clean --debug` |
| **Analyze Code** | `./analyze.sh --all` |

---

## 🎯 Using Make (Alternative)

If you prefer `make` over CMake:

```bash
# Build
make

# Build and run
make run

# Release build
make release run

# Clean
make clean

# With sanitizers
make asan run
```

---

## 🔍 When Something Goes Wrong

### Segfault / Crash?

```bash
# 1. Run with sanitizer
./build.sh --asan --run

# 2. If that doesn't show it, use GDB
./debug.sh --gdb
(gdb) run
(gdb) backtrace    # Shows exactly where it crashed!
```

### Slow Performance?

```bash
# Build in release mode
./build.sh --release --run
```

### Memory Leak?

```bash
# Address Sanitizer
./build.sh --asan --run

# Or Valgrind for detailed analysis
./debug.sh --valgrind
```

---

## 🎓 Learning GDB

### Essential Commands

```gdb
run (r)          Start the program
break (b)        Set breakpoint
next (n)         Step over (next line)
step (s)         Step into (enter function)
continue (c)     Continue execution
print (p)        Print variable value
backtrace (bt)   Show call stack
list (l)         Show source code
quit (q)         Exit GDB
```

### Example Debug Session

```bash
$ gdb ./BlackHoleTravelSimulator

(gdb) break main.cpp:500         # Break at line 500
(gdb) run                        # Start program
(gdb) print ship.position.x      # Print position
(gdb) next                       # Go to next line
(gdb) backtrace                  # See call stack
(gdb) continue                   # Keep running
(gdb) quit                       # Exit
```

---

## 🌟 Pro Tips

### 1. Fastest Build
```bash
./build.sh --release --clang
```

### 2. Best Bug Detection
```bash
./build.sh --asan --ubsan --run
```

### 3. Clean Rebuild
```bash
./build.sh --clean --debug
```

### 4. Parallel Build (Faster!)
```bash
# Edit build.sh, change -j4 to -j8 or -j16
cmake --build . -j16
```

### 5. Profile Performance
```bash
# Linux only
perf record -g ./BlackHoleTravelSimulator
perf report
```

---

## 📁 What Each File Does

| File | Purpose |
|------|---------|
| `CMakeLists.txt` | Build configuration |
| `build.sh` | Build script (Linux/Mac/GitBash) |
| `build.bat` | Build script (Windows cmd) |
| `debug.sh` | Debug with GDB/LLDB/Valgrind |
| `analyze.sh` | Static analysis tools |
| `Makefile` | Alternative to CMake |

---

## 🆘 Quick Troubleshooting

### Error: "SFML not found"
```bash
# Edit CMakeLists.txt line 42:
set(SFML_DIR "C:/path/to/your/SFML/lib/cmake/SFML")
```

### Error: "glad.c not found"
```bash
# Edit CMakeLists.txt line 46:
set(GLAD_SOURCE "C:/path/to/glad/src/glad.c")
```

### Build fails?
```bash
# Try clean build
./build.sh --clean --debug

# Check compiler is installed
gcc --version
clang --version
```

---

## 🎉 You're Ready!

**Most Common Workflow:**

```bash
# 1. Build
./build.sh --debug

# 2. Run
cd build
./BlackHoleTravelSimulator

# 3. If crashes, debug
cd ..
./debug.sh --gdb
```

**For detailed info, see:** `TERMINAL_BUILD_GUIDE.md`

---

**Happy Coding!** 🚀🔧
