# Planet Surface Textures & Landing System

This note tracks visual texture work and landing-related behavior in one place.

## Implementation Summary

This document describes the enhanced planet textures and landing system added to the space simulation.

---

## 1. ENHANCED PLANET SURFACE TEXTURES

### Earth Texture (Realistic)
**File**: `TextureLoader.cpp:128-171`

**Features**:
- **Continents & Oceans**: Multi-scale Perlin noise creates realistic landmasses
- **Ocean Depth Variation**:
  - Deep ocean (dark navy blue)
  - Medium depth (standard blue)
  - Shallow coastal waters (cyan/turquoise)
- **Terrain Elevation**:
  - Low elevation: Sandy/desert terrain (tan/beige)
  - Mid elevation: Green vegetation
  - High elevation: Brown/gray mountains with snow-capped peaks
- **Polar Ice Caps**: Bright white/blue ice at latitudes > 85°
- **Clouds**: Wispy scattered cloud formations (80% opacity blend)

**Technical Details**:
- Uses 5-octave FBM (Fractal Brownian Motion) for continent generation
- Latitude-based polar cap calculation
- Height-based terrain classification with smooth transitions
- 512x512 RGB texture resolution

### Mars Texture (Realistic)
**File**: `TextureLoader.cpp:173-206`

**Features**:
- **Varied Terrain Colors**:
  - High terrain: Bright orange/tan (like Olympus Mons)
  - Mid elevation: Rusty red iron oxide surface
  - Low terrain: Dark ancient lava plains
- **Valles Marineris**: Massive equatorial canyon system (dark band)
- **Polar Ice Caps**: CO2 and water ice (pinkish-white) at poles > 82° latitude
- **Heavy Cratering**:
  - Large craters with depth and shadows
  - Small frequent craters from micrometeorites
- **Dust Storms**: Occasional bright dust cloud regions

**Technical Details**:
- Multi-layer terrain synthesis (base + detail + dust)
- Equatorial canyon at latitude < 15°
- Dual-size crater generation (large 12% threshold, small 8% threshold)
- Elevation-based color mapping

### Moon Texture (Realistic)
**File**: `TextureLoader.cpp:403-424`

**Features**:
- **Maria (Dark Seas)**: Large basaltic plains covering ~30% of surface
- **Highlands**: Bright ancient crustal material
- **Heavy Cratering**: Three scales of craters
  - Large: Tycho, Copernicus-style with bright ejecta rays
  - Medium: Frequent smaller impact craters
  - Small: Micrometeorite pitting
- **Crater Rays**: Bright radial streaks from fresh impacts
- **Rilles**: Sinuous lava channels in maria regions
- **No Atmosphere**: Sharp contrast, no clouds or haze

**Technical Details**:
- Terrain threshold: < 0.42 = maria, > 0.42 = highlands
- Three-tier cratering system with different frequencies
- Ray generation for fresh craters (>88% noise threshold)
- Rille channels in maria using narrow noise bands

### Moon Added to Solar System
**File**: `SolarSystemFactory.cpp:26-28`

**Properties**:
- Mass: 0.012 (Earth = 1.0)
- Radius: 0.27 units
- Position: 3 units from Earth
- Orbital velocity: Earth velocity + 1.0 (synchronized orbit)
- Color: Light gray (0.7, 0.7, 0.7)
- Texture: Procedural Moon texture (512x512)

---

## 2. LANDING SYSTEM

### State Machine
**File**: `Spaceship.hpp:13-19`

```cpp
enum class LandingState {
    FLYING,      // Normal flight - no landing possible
    APPROACHING, // Within landing range, velocity low enough
    LANDED       // On surface, physics disabled
};
```

### Landing Parameters
- **Landing Distance Threshold**: 3.0 × (ship radius + planet radius)
- **Landing Velocity Threshold**: 5.0 units/second (max safe landing speed)
- **Landable Bodies**: Earth, Moon, Mars, Mercury (solid surfaces only)
- **Non-landable**: Jupiter, Saturn, Uranus, Neptune (gas giants), Sun, Black Hole

### Key Controls
- **B Key**: Attempt landing (when in APPROACHING state)
- **N Key**: Take off from surface (when LANDED)

### Proximity Detection
**File**: `Spaceship.cpp:230-258`

**Logic**:
1. Check all celestial bodies each frame
2. Calculate surface distance (distance - planet radius - ship radius)
3. Check if within landing range AND velocity < threshold
4. Transition to APPROACHING if conditions met
5. Transition back to FLYING if moving away or too fast

**Method**: `spaceship.checkLandingProximity(bodies)`

### Landing Sequence
**File**: `Spaceship.cpp:260-282`

**Steps**:
1. Verify ship is in APPROACHING state
2. Verify body is landable (solid surface)
3. Calculate landing offset (position relative to planet center)
4. Lock ship to surface at: planet_radius + ship_radius + 0.1
5. Zero out velocity and acceleration
6. Set state to LANDED
7. Store landed body reference

**Console Output**: "Successfully landed on [Planet Name]!"

### Takeoff Sequence
**File**: `Spaceship.cpp:284-301`

**Steps**:
1. Verify ship is in LANDED state
2. Calculate "away from planet" direction vector
3. Apply launch impulse: 3.0 units/s away from surface
4. Reset landed body reference to nullptr
5. Transition state to FLYING

**Console Output**: "Taking off from [Planet Name]!"

### Surface Lock (While Landed)
**File**: `Spaceship.cpp:28-42`

**Behavior**:
- Position locked to planet surface every frame
- Velocity forced to zero
- Acceleration forced to zero
- Physics engine bypassed for ship (but still applied to planets)

**Technical**:
```cpp
Vec3 planetPos = landedOn->getPhysicsBody().position;
Vec3 toPlanet = (planetPos - position).normalize();
float surfaceDistance = planet_radius + ship_radius + 0.1f;
position = planetPos - toPlanet * surfaceDistance;
```

---

## 3. INTEGRATION WITH MAIN LOOP

### Physics Update (main.cpp:689-717)
```cpp
if (ship.getLandingState() != LandingState::LANDED) {
    physics.update(deltaTime);  // Normal physics
} else {
    // Ship landed: update planets only, skip ship
    // ... apply gravity between celestial bodies
}

ship.checkLandingProximity(bodies);  // Check every frame
```

### Input Handling (main.cpp:528-574)
**B Key Press**:
- Find closest landable body
- Call `spaceship.attemptLanding(body)`
- Show error messages if not in range

**N Key Press**:
- Call `spaceship.takeoff()`
- Show error message if not landed

### User Feedback
- Console messages for landing/takeoff
- Console warnings if trying to land on gas giants or when moving too fast
- State transitions logged to console

---

## 4. TESTING INSTRUCTIONS

### Test Earth Landing
1. Launch simulation
2. Navigate toward Earth (blue planet with continents)
3. Slow down to < 5 units/s (press K for reverse thrust)
4. Get within ~2-3 units of surface
5. Console should show "Approaching" messages
6. Press **B** to land
7. Observe: Ship locks to surface, velocity = 0
8. Press **N** to take off
9. Observe: Ship launches upward at 3 units/s

### Test Moon Landing
1. Navigate to Moon (gray cratered surface, orbiting Earth)
2. Match Moon's orbital velocity
3. Slow approach to < 5 units/s
4. Press **B** when close
5. Land on lunar surface

### Test Mars Landing
1. Navigate to Mars (rusty red planet with dark canyon)
2. Slow down and approach surface
3. Press **B** to land
4. Test takeoff with **N**

### Test Invalid Landings
1. Try landing on Jupiter (gas giant)
   - **Expected**: Console error: "no solid surface"
2. Try landing while moving > 5 units/s
   - **Expected**: Console error: "moving too fast"
3. Press **B** while far from any planet
   - **Expected**: Console error: "not in approach range"

---

## 5. TECHNICAL ARCHITECTURE

### Class Modifications

**Spaceship.hpp**:
- Added `LandingState` enum
- Added `landedOn` pointer to track current body
- Added `landingDistThreshold`, `landingVelThreshold` parameters
- Added methods: `checkLandingProximity()`, `attemptLanding()`, `takeoff()`, `canLandOn()`

**Spaceship.cpp**:
- Modified `update()` to lock position when landed
- Implemented landing FSM logic
- Added landable body validation

**TextureLoader.cpp**:
- Completely rewrote `generateEarthTexture()` with realistic features
- Completely rewrote `generateMarsTexture()` with Valles Marineris and polar caps
- Completely rewrote `generateMoonTexture()` with maria, highlands, and heavy cratering

**SolarSystemFactory.cpp**:
- Added Moon celestial body between Earth and Mars
- Configured Moon orbit around Earth

**main.cpp**:
- Added Moon texture generation call
- Added B/N key event handlers
- Added proximity check in main loop
- Modified physics update to skip ship when landed

### Dependencies
- Forward declaration of `CelestialBody` in `Spaceship.hpp`
- Full include of `CelestialBody.hpp` in `Spaceship.cpp`
- Vec3 math for surface normal calculations
- Physics engine integration for landing impulse

---

## 6. VISUAL RESULTS

### Earth
- Realistic blue oceans with depth variation
- Green/brown continents with mountain ranges
- White polar ice caps at top/bottom
- Wispy white cloud formations
- Coastal cyan shallow waters

### Mars
- Rusty red terrain (iron oxide)
- Dark horizontal canyon (Valles Marineris)
- Bright tan high terrain (mountains)
- White/pink polar ice caps
- Dark craters scattered across surface
- Occasional bright dust storm regions

### Moon
- Gray highlands (bright regions)
- Dark maria "seas" (ancient lava)
- Dense cratering at multiple scales
- Bright ejecta rays from fresh craters
- Sharp contrast (no atmosphere)
- Dark sinuous rilles in maria

---

## 7. FUTURE ENHANCEMENTS

Possible improvements:
- **Surface EVA Mode**: Walk around on foot when landed
- **Landing Gear Animation**: Deploy landing struts
- **Surface Science**: Collect samples, plant flags
- **Launch Pad Markers**: Designated landing zones
- **Landing Guidance**: Approach vector visualization
- **Atmospheric Entry Effects**: Heat glow, camera shake
- **City Lights**: Nightside Earth illumination
- **Normal Maps**: 3D bump mapping for terrain depth

---

## CONTROLS SUMMARY

### New Controls
- **B**: Land on nearby planet (when in approach range)
- **N**: Take off from surface (when landed)

### Existing Controls (unchanged)
- **I/↑**: Forward thrust
- **K/↓**: Reverse thrust
- **J/←**: Rotate left
- **L/→**: Rotate right
- **U**: Pitch up
- **O**: Pitch down
- **C**: Cycle camera modes
- **Space**: Toggle velocity vectors
- **P**: Pause/unpause simulation

---

## CODE QUALITY

- **No warnings**: All code compiles cleanly
- **Type safety**: Strong typing with enum class
- **Memory safety**: Raw pointer used only for non-owning reference
- **Null checks**: All pointer dereferences protected
- **State validation**: FSM transitions validated before execution
- **User feedback**: Console messages for all actions and errors
- **Performance**: O(N) proximity check, negligible overhead

---

**Implementation Date**: March 12, 2026
**Files Modified**: 5 (TextureLoader.cpp, Spaceship.hpp/cpp, SolarSystemFactory.cpp, main.cpp, CMakeLists.txt)
**Lines Added**: ~450
**Status**: Complete and ready for testing
