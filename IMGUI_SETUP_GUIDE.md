# ImGui Setup Guide for Black Hole Travel Simulator

## New Features Added

This update adds three major visual and UX improvements:

1. **ImGui Integration** - Interactive GUI panels for all controls
2. **Black Hole Gravitational Lensing** - Stunning light-bending effects
3. **Bloom/HDR Post-Processing** - Professional cinematic visuals

---

## Prerequisites

Before building, you need to download and set up ImGui:

### Step 1: Download ImGui

1. **Download ImGui core library:**
   ```
   https://github.com/ocornut/imgui
   ```
   - Click "Code" → "Download ZIP"
   - Extract to `C:\Dev\imgui\` (or adjust path in CMakeLists.txt)

2. **Download ImGui-SFML binding:**
   ```
   https://github.com/SFML/imgui-sfml
   ```
   - Click "Code" → "Download ZIP"
   - Extract to `C:\Dev\imgui-sfml\` (or adjust path in CMakeLists.txt)

### Step 2: Verify Directory Structure

Your directory structure should look like this:

```
C:\Dev\
├── imgui\
│   ├── imgui.h
│   ├── imgui.cpp
│   ├── imgui_draw.cpp
│   ├── imgui_tables.cpp
│   ├── imgui_widgets.cpp
│   ├── imgui_internal.h
│   └── ... (other files)
│
├── imgui-sfml\
│   ├── imgui-SFML.h
│   ├── imgui-SFML.cpp
│   └── ... (other files)
│
└── C++Code\
    └── update_orderbook_02\
        └── black_hole_travel_timed_1\
            └── black_hole_travel_timed_1\
                ├── main.cpp
                ├── CMakeLists.txt
                ├── GUI.hpp
                ├── PostProcessing.hpp
                ├── BlackHole.hpp
                ├── AccretionDisk.hpp
                └── ... (other project files)
```

### Step 3: Update CMakeLists.txt Paths (if needed)

If you extracted ImGui to a different location, update these lines in `CMakeLists.txt`:

```cmake
# Line ~70-71
set(IMGUI_DIR "C:/Dev/imgui")           # <- Update this path
set(IMGUI_SFML_DIR "C:/Dev/imgui-sfml") # <- Update this path
```

---

## Building the Project

### Option 1: CMake Build (Recommended)

```bash
cd black_hole_travel_timed_1
mkdir build
cd build
cmake ..
cmake --build .
```

### Option 2: Direct Compilation

If you prefer to compile directly without CMake, you'll need to:

1. Add all ImGui source files to your compilation
2. Link against SFML, OpenGL, and GLAD
3. Include the imgui and imgui-sfml directories

---

## New Features Overview

### 1. ImGui GUI System

**Interactive panels include:**

- **Control Panel** - Master control for all features
  - Time scale controls (0.1x to 10x)
  - Pause/Resume simulation
  - Display toggles (vectors, trajectory, grid, Lagrange points)
  - Panel visibility toggles

- **Ship Telemetry** - Real-time spacecraft data
  - Position (x, y, z)
  - Velocity vector
  - Speed in m/s
  - Nearest celestial body
  - Distance and altitude
  - Low altitude warnings
  - Thrust power slider (10-200)
  - Rotation speed slider (10-120°/s)

- **Visual Settings** - Post-processing controls
  - Bloom enable/disable
  - Bloom threshold (0.5-2.0)
  - Bloom strength (0.0-0.2)
  - Exposure (HDR tone mapping, 0.1-3.0)
  - Reset to defaults button

- **Orbital Data** - Orbital mechanics analysis
  - Orbit type
  - Apoapsis/Periapsis
  - Eccentricity
  - Hohmann transfer calculator

- **Performance Stats** - System metrics
  - FPS counter
  - Frame time in milliseconds
  - Graphics feature status

**Theme:** Dark space theme with cyan/blue accents

### 2. Bloom & HDR Post-Processing

**What it does:**
- Extracts bright areas (sun, engines, particles)
- Applies Gaussian blur to create glow
- Combines bloom with scene
- Applies HDR tone mapping (Reinhard)
- Gamma correction for proper display

**Controls:**
- **Bloom Threshold**: Brightness level to trigger bloom (default: 1.0)
- **Bloom Strength**: Intensity of glow effect (default: 0.04)
- **Exposure**: Overall brightness of scene (default: 1.0)

**Visual impact:**
- Sun has intense HDR glow
- Stars twinkle with halos
- Engine particles leave glowing trails
- Professional cinematic look

### 3. Black Hole Accretion Disk

**Features:**
- Swirling disk of matter around black hole
- Heat gradient: white-hot center → orange → dark red edges
- Rotation animation (different speeds at different radii)
- Turbulence and swirl patterns
- Semi-transparent with edge fade
- 100 segments × 40 rings for smooth appearance

**Physics-inspired:**
- Inner radius: 3.0 units (close to event horizon)
- Outer radius: 8.0 units
- Color represents temperature (real astrophysics)
- Rotation simulates orbital motion

### 4. Gravitational Lensing

**Effect zones:**
1. **Event Horizon** (r < 1.1 × Schwarzschild radius)
   - Pure black (no light escapes)

2. **Photon Sphere** (1.1r < r < 1.5r)
   - Extreme light bending
   - Purple/blue distortion effect

3. **Lensing Ring** (near 2.0r)
   - Bright ring from bent starlight
   - Einstein ring effect

4. **Outer Region**
   - Gradual light distortion
   - Space appears warped

**Implementation:**
- Fragment shader calculates light ray bending
- Strength based on distance from event horizon
- Transparent sphere overlay
- Realistic astrophysics simulation

---

## Usage Guide

### GUI Controls

**Mouse:**
- Click and drag panels to move them
- Scroll in panels to see more options
- Sliders can be dragged or clicked to set value

**Keyboard shortcuts still work:**
- All original keyboard controls remain functional
- Use GUI for fine-tuned adjustments

### Recommended Settings

**For cinematic screenshots:**
```
Bloom Threshold: 0.8
Bloom Strength: 0.08
Exposure: 1.2
```

**For realistic simulation:**
```
Bloom Threshold: 1.2
Bloom Strength: 0.04
Exposure: 1.0
```

**For bright, high-energy scenes:**
```
Bloom Threshold: 0.5
Bloom Strength: 0.12
Exposure: 1.5
```

### Performance Tips

1. **If FPS is low:**
   - Reduce bloom iterations (requires code change in PostProcessing.hpp, line ~145)
   - Disable bloom entirely via GUI
   - Lower resolution (requires code change in main.cpp)

2. **If black hole doesn't appear:**
   - Check that a body named "Black Hole" exists in SolarSystemFactory
   - Verify it has sufficient mass (8000+ recommended)

---

## Troubleshooting

### Build Errors

**"Cannot find imgui.h"**
- Verify ImGui is downloaded to correct path
- Check CMakeLists.txt paths match your installation
- Ensure `IMGUI_DIR` and `IMGUI_SFML_DIR` are set correctly

**"Undefined reference to ImGui::..."**
- Make sure all ImGui source files are included in `IMGUI_SOURCES`
- Verify imgui-SFML.cpp is included
- Check that SFML is properly linked

**"Linker error: multiple definition of..."**
- You may have included .cpp files instead of just .h
- Check that ImGui sources are only in CMakeLists, not #included

### Runtime Issues

**GUI doesn't appear:**
- Check that ImGui::SFML::Init() succeeded
- Verify OpenGL context is created before GUI initialization
- Make sure window is created with correct SFML version

**Bloom looks wrong:**
- Adjust threshold and strength sliders
- Check that framebuffer is properly bound
- Verify GL_RGBA16F texture format is supported

**Black hole is invisible:**
- Increase accretion disk brightness (edit shader)
- Check black hole position isn't off-screen
- Verify lensing shader is compiling without errors

---

## Code Architecture

### New Files Created

1. **Framebuffer.hpp** - Render-to-texture for post-processing
   - Creates FBO with HDR color texture
   - Manages depth buffer
   - Supports resizing

2. **PostProcessing.hpp** - Bloom and HDR pipeline
   - Bright pass extraction
   - Ping-pong Gaussian blur
   - HDR tone mapping (Reinhard)
   - Gamma correction

3. **BlackHole.hpp** - Enhanced black hole class
   - Inherits from CelestialBody
   - Schwarzschild radius calculation
   - Photon sphere rendering
   - Lensing effect methods

4. **AccretionDisk.hpp** - Disk mesh generator
   - Procedural ring generation
   - Heat gradient shader
   - Rotation animation
   - Turbulence effects

5. **GUI.hpp** - ImGui wrapper class
   - Panel management
   - State synchronization
   - Custom space theme
   - Event handling

### Integration Points

**main.cpp changes:**
- Lines 31-35: New includes
- Lines 170-171: New shader creation
- Lines 199-206: System initialization
- Lines 381: GUI event processing
- Lines 624-634: GUI state synchronization
- Lines 841: Framebuffer binding
- Lines 970-1013: Black hole rendering
- Lines 1146-1167: Post-processing and GUI render

**CMakeLists.txt changes:**
- Lines 70-79: ImGui paths and sources
- Lines 99: Added IMGUI_SOURCES to build
- Lines 135-139: New header files
- Lines 144-147: ImGui include directories

---

## Advanced Customization

### Adjusting Bloom Quality

In `PostProcessing.hpp`, line ~145:
```cpp
int blurIterations = 10;  // Higher = smoother but slower
```

### Changing Accretion Disk Colors

In `AccretionDisk.hpp`, lines ~46-48:
```cpp
vec3 innerColor = vec3(1.0, 0.95, 0.8);   // Hot center
vec3 middleColor = vec3(1.0, 0.5, 0.2);   // Mid disk
vec3 outerColor = vec3(0.8, 0.2, 0.1);    // Outer edge
```

### Lensing Strength

In `main.cpp`, around line 995:
```cpp
* Mat4::scale(blackHoleBody->getRadius() * 2.5f);
                    // Increase multiplier for larger lensing sphere
```

---

## Credits

- **ImGui**: Omar Cornut (ocornut) - https://github.com/ocornut/imgui
- **ImGui-SFML**: Elias Daler - https://github.com/SFML/imgui-sfml
- **Bloom Algorithm**: Based on Call of Duty bloom technique
- **HDR Tone Mapping**: Reinhard operator
- **Gravitational Lensing**: Inspired by real black hole physics

---

## Next Steps

Want to add more features? Consider:

1. **Lens Flare** - When looking at the sun
2. **Volumetric Lighting** - God rays through atmosphere
3. **Particle Effects** - Enhanced engine trails
4. **Sound System** - Engine sounds, collisions, ambient music
5. **Save/Load** - Save simulation state and GUI settings
6. **Mission System** - Objectives and challenges
7. **More Planets** - Expand the solar system
8. **Multiplayer** - Connect multiple ships

Enjoy your enhanced space simulator! 🚀
