# Quick Start: New Features Setup

## ⚡ 5-Minute Setup Guide

Follow these steps to get the new features running:

---

## Step 1: Download ImGui (2 minutes)

### Option A: Using Git (Recommended)
```bash
cd C:\Dev
git clone https://github.com/ocornut/imgui.git
git clone https://github.com/SFML/imgui-sfml.git
```

### Option B: Manual Download
1. Go to https://github.com/ocornut/imgui
2. Click **Code** → **Download ZIP**
3. Extract to `C:\Dev\imgui\`

4. Go to https://github.com/SFML/imgui-sfml
5. Click **Code** → **Download ZIP**
6. Extract to `C:\Dev\imgui-sfml\`

---

## Step 2: Verify Paths (30 seconds)

Check that these files exist:

Use forward slashes in CMake paths if Windows backslashes cause escaping issues.

```
✅ C:\Dev\imgui\imgui.h
✅ C:\Dev\imgui\imgui.cpp
✅ C:\Dev\imgui-sfml\imgui-SFML.h
✅ C:\Dev\imgui-sfml\imgui-SFML.cpp
```

If your ImGui is in a different location, edit `CMakeLists.txt` lines 70-71:
```cmake
set(IMGUI_DIR "C:/Dev/imgui")           # <- Your path here
set(IMGUI_SFML_DIR "C:/Dev/imgui-sfml") # <- Your path here
```

---

## Step 3: Build the Project (2 minutes)

### Using CMake:
```bash
cd black_hole_travel_timed_1
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Using Visual Studio:
1. Open `black_hole_travel_timed_1.sln`
2. Right-click project → **Build**

---

## Step 4: Run and Explore! (30 seconds)

Launch the executable:
```bash
./BlackHoleTravelSimulator
```

**What you should see:**
- ✅ Main 3D scene
- ✅ GUI panels on screen
- ✅ Sun has intense glow (bloom)
- ✅ Black hole has swirling accretion disk

---

## 🎮 First Things to Try

### 1. Test the GUI (30 seconds)
- Click and drag the **Control Panel** window
- Click **Pause** button - simulation should freeze
- Drag **Time Scale** slider - see speed change
- Click **Visual Settings** panel

### 2. Adjust Bloom (30 seconds)
- In **Visual Settings** panel:
  - Drag **Bloom Strength** to 0.10
  - Drag **Exposure** to 1.5
- Notice the sun gets much brighter and more dramatic

### 3. Explore Black Hole (1 minute)
- Press **I** to thrust forward
- Use **J/L** to rotate toward black hole
- Watch for:
  - ✨ Swirling orange/red accretion disk
  - 🌀 Space distortion near event horizon
  - ⚠️ Warning in Ship Telemetry panel

### 4. Check Performance (30 seconds)
- Open **Performance** panel
- Note your FPS
- Toggle **Bloom** on/off to see impact

---

## 🎨 Recommended First Settings

For the best visual experience:

**In Visual Settings panel:**
```
Bloom Threshold: 1.0
Bloom Strength: 0.06
Exposure: 1.2
```

**In Control Panel:**
```
Time Scale: 1.0x
Show Velocity Vectors: ON
Show Coordinate Grid: OFF (toggle with G later)
```

---

## ❓ Troubleshooting

### Build fails with "Cannot find imgui.h"
**Fix:** Check ImGui paths in CMakeLists.txt (lines 70-71)

### Build fails with "Undefined reference to ImGui::..."
**Fix:** Make sure imgui.cpp, imgui_draw.cpp, imgui_tables.cpp, imgui_widgets.cpp, and imgui-SFML.cpp are all present

### No GUI appears at runtime
**Fix:**
1. Check console for errors
2. Verify OpenGL 4.6 support
3. Try updating graphics drivers

### FPS is very low (<30)
**Fix:**
1. Open Visual Settings panel
2. Uncheck "Enable Bloom"
3. Or reduce Bloom Strength to 0.02

### Black hole looks wrong
**Fix:**
1. Navigate closer (press I to thrust)
2. Make sure you're looking at the "Black Hole" body
3. Time scale might be too slow - set to 1.0x

---

## 🎯 What's Different?

### Before Update:
```
[Console text only]
Sun (basic)
Black hole (black sphere)
Keyboard controls
```

### After Update:
```
[Interactive GUI panels]
Sun (intense HDR glow)
Black hole (accretion disk + lensing)
GUI + Keyboard controls
```

---

## 📚 Next Steps

Once everything is working:

1. **Read** `NEW_FEATURES_UPDATE.md` for full details
2. **Experiment** with GUI sliders
3. **Try** different visual settings
4. **Explore** the black hole up close
5. **Check** orbital mechanics features

---

## 🚨 Known Issues

1. **Lensing sphere not fully rendered** - Visual effect is there but simplified
2. **GUI settings don't save** - Will reset when you restart (planned feature)
3. **Fixed resolution** - Currently hardcoded to 1280×720

These are minor and don't affect the core experience.

---

## 💡 Tips & Tricks

### For Screenshots:
1. Close all GUI panels (click X on each)
2. Increase bloom strength to 0.15
3. Navigate to interesting angle
4. Press PrintScreen

### For Performance:
1. Disable bloom if FPS < 60
2. Reduce time scale instead of particle count
3. Close unnecessary GUI panels

### For Exploration:
1. Use Chase camera mode (press C)
2. Enable coordinate grid (press G)
3. Watch ship telemetry for warnings
4. Try Hohmann transfer (press H)

---

## 🎓 Learning the Features

**Spend 2 minutes on each:**

1. **GUI System** (5 minutes)
   - Open each panel
   - Try every slider
   - Click every button

2. **Bloom Effects** (5 minutes)
   - Toggle on/off
   - Adjust threshold
   - Adjust strength
   - Change exposure

3. **Black Hole** (5 minutes)
   - Navigate close
   - Observe disk rotation
   - Look for lensing
   - Check event horizon

**Total exploration time: ~15 minutes**

---

## ✅ Success Checklist

You know it's working when:

- [ ] GUI panels are visible and draggable
- [ ] Sun has bright, glowing halo
- [ ] Bloom can be toggled on/off
- [ ] Black hole has orange/red disk
- [ ] Accretion disk rotates slowly
- [ ] Ship telemetry shows position
- [ ] Time scale slider changes speed
- [ ] Pause button freezes simulation
- [ ] Performance stats show FPS
- [ ] No console errors

---

## 🆘 Need Help?

1. **Check** `IMGUI_SETUP_GUIDE.md` for detailed setup
2. **Read** `NEW_FEATURES_UPDATE.md` for technical details
3. **Review** console output for errors
4. **Verify** all dependencies are installed

---

## 🎉 You're Ready!

If you've completed the checklist above, you're all set to enjoy the enhanced simulator!

**Have fun exploring the cosmos!** 🚀✨

---

*Estimated total setup time: 5-10 minutes (depending on download speed)*
