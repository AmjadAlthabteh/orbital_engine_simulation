# BLACK HOLE TRAVEL SIMULATOR - NEW ADVANCED FEATURES

Use this guide as a feature checklist when validating a new build locally.

## Table of Contents
1. [Achievement System](#achievement-system)
2. [Science & Data Collection](#science-data-collection)
3. [Waypoint Navigation](#waypoint-navigation)
4. [Advanced Telemetry](#advanced-telemetry)
5. [Cosmic Event System](#cosmic-event-system)
6. [Enhanced Statistics](#enhanced-statistics-system)
7. [Planet Information Panel](#planet-information-panel)
8. [Advanced HUD Overlay](#advanced-hud-overlay)
9. [Resource Management System](#resource-management-system)
10. [Damage & Repair System](#damage-repair-system)
11. [Mission System](#mission-system)
12. [Save/Load System](#saveload-system)
13. [Audio System](#audio-system)
14. [Enhanced Console Logging](#enhanced-console-logging)
15. [Keyboard Shortcuts](#keyboard-shortcuts)
16. [Tips & Strategies](#tips-strategies)

---

## Achievement System

### Overview
Unlock achievements by completing various challenges and milestones during your space journey!

### Achievement Categories

#### Speed Achievements
- **Space Cadet** - Reach 20 m/s (10 points)
- **Cosmic Pilot** - Reach 50 m/s (25 points)
- **Speed Demon** - Reach 100 m/s (50 points)
- **Lightspeed Legend** - Reach 150 m/s (100 points)

#### Exploration Achievements
- **First Contact** - Visit your first planet (15 points)
- **Space Tourist** - Visit 3 different planets (30 points)
- **Cosmic Voyager** - Visit 5 different planets (60 points)
- **Grand Tour** - Visit all 8 planets (150 points)

#### Survival Achievements
- **Close Call** - Survive altitude below 5 units (20 points)
- **Daredevil** - Survive 5 close calls (50 points)
- **Event Horizon Survivor** - Approach black hole within 15 units (75 points)

#### Mastery Achievements
- **Smooth Landing** - Successfully land on a planet (40 points)
- **Orbital Mechanic** - Calculate a Hohmann transfer (35 points)
- **Precision Pilot** - Match velocity with target within 5 m/s (45 points)

#### Distance Achievements
- **Star Wanderer** - Travel 500 units total (25 points)
- **Deep Space Voyager** - Travel 2000 units total (75 points)
- **Interstellar Legend** - Travel 5000 units total (150 points)

#### Secret Achievements (Hidden)
- **Too Close to the Sun** - Get within 10 units of the Sun (50 points)
- **Zen Master** - Travel at exactly 0 m/s for 5 seconds (30 points)

### How to Use
- **Press F2** to view your achievement progress
- Achievements unlock automatically when conditions are met
- Total points and unlock notifications displayed in console

---

## Science & Data Collection

### Overview
Scan celestial bodies to collect scientific data and earn science points!

### Science Experiments by Planet

#### Earth (45 science points total)
- Visual Survey (15 pts) - High-resolution imaging
- Atmospheric Scan (20 pts) - Composition analysis
- Gravity Measurement (10 pts) - Field mapping

#### Mars (73 science points total)
- Visual Survey (18 pts) - Surface geology
- Atmospheric Scan (25 pts) - CO2 analysis
- Close Flyby (30 pts) - Low-altitude data

#### Jupiter (97 science points total)
- Visual Survey (22 pts) - Great Red Spot
- Atmospheric Scan (35 pts) - Gas composition
- Magnetic Field (40 pts) - Magnetosphere study

#### Black Hole (250 science points! - EXTREME RISK)
- Gravity Measurement (100 pts) - Event horizon data
- Close Flyby (150 pts) - Accretion disk analysis

#### Sun (125 science points - HIGH RISK)
- Visual Survey (50 pts) - Corona imaging
- Close Flyby (75 pts) - Solar wind measurement

### How to Use
1. **Get within 50 units** of your target planet
2. **Press F7** to initiate scan
3. **Wait 3 seconds** for scan completion
4. Receive science points and experiment data
5. **Press F3** to view science summary

### Tips
- Each planet can only be scanned once
- Closer distances improve data quality
- High-risk targets (Sun, Black Hole) offer huge rewards
- Use science points to track exploration progress

---

## Waypoint Navigation System

### Overview
Create custom navigation markers anywhere in space for easy navigation!

### Features
- Create unlimited waypoints
- Automatically sorted by distance
- Descriptive labels and positions
- Quick navigation reference

### Preset Waypoints
Three waypoints are created automatically:
1. **Origin Point** (0, 0, 0) - Center of solar system
2. **Safe Zone** (100, 0, 100) - Safe observation point
3. **Mission Start** (-20, 0, 50) - Default ship position

### How to Use
- **Press F5** to list all waypoints
- **Press F6** to create waypoint at current position
- Waypoints show name, position, and distance
- Use for mission planning and navigation

### Example Use Cases
- Mark discovered anomalies
- Save interesting locations
- Plan exploration routes
- Create navigation checkpoints
- Mark fuel stops or safe zones

---

## Advanced Telemetry System

### Overview
Detailed real-time analysis of ship performance, forces, and orbital mechanics!

### Data Provided

#### Position & Motion
- 3D position coordinates (X, Y, Z)
- Velocity vector components
- Current speed (m/s)

#### Acceleration & G-Forces
- Current acceleration (m/s²)
- Real-time G-force measurement
- Peak G-force (max recorded)
- Average G-force (1-minute rolling)
- **WARNING**: G-forces > 8G risk pilot unconsciousness!

#### Proximity Analysis
- Nearest celestial body
- Distance to nearest body
- Altitude above surface
- **Color-coded warnings**:
  - 🔴 Red (< 5 units) - COLLISION WARNING
  - 🟡 Yellow (< 15 units) - CAUTION
  - 🟢 Green (> 15 units) - SAFE

#### Orbital Parameters
- Radial velocity (toward/away from body)
- Tangential velocity (orbital component)
- Orbit status (ASCENDING/DESCENDING/STABLE)

### How to Use
- **Press F4** for comprehensive telemetry display
- Monitor during high-G maneuvers
- Use for precision orbital insertions
- Track performance statistics

### Safety Thresholds
- **< 5G**: Normal operations
- **5-8G**: High stress on structure
- **> 8G**: EXTREME - Risk to pilot!

---

## Cosmic Event System

### Overview
Random space phenomena occur during your journey, adding challenges and opportunities!

### Event Types

#### Solar Flare
- **Duration**: 15 seconds
- **Effect**: Elevated radiation levels
- **Intensity**: 70%
- **Occurrence**: Near Sun

#### Meteor Shower
- **Duration**: 20 seconds
- **Effect**: Debris field, micrometeorite risk
- **Intensity**: 50%
- **Occurrence**: Random

#### Gravitational Anomaly
- **Duration**: 12 seconds
- **Effect**: Navigation affected
- **Intensity**: 60%
- **Occurrence**: Near massive objects

#### Cosmic Ray Burst
- **Duration**: 8 seconds
- **Effect**: High-energy particles
- **Intensity**: 40%
- **Occurrence**: Deep space

#### Stellar Phenomenon
- **Duration**: 25 seconds
- **Effect**: Perfect for science observations!
- **Intensity**: 80%
- **Occurrence**: Rare

#### Time Dilation Effect
- **Duration**: 10 seconds
- **Effect**: Temporal distortion
- **Intensity**: 90%
- **Occurrence**: Near black hole

#### Magnetic Storm
- **Duration**: 18 seconds
- **Effect**: Compass unreliable
- **Intensity**: 60%
- **Occurrence**: Magnetic field areas

### How to Use
- Events trigger automatically (20% chance every 30 seconds)
- Maximum 2 concurrent events
- Event notifications display in console
- **Press F8** to manually trigger random event (testing)
- Duration and intensity shown on trigger
- Events provide gameplay variety and realism

---

## Enhanced Statistics System

### Overview
Comprehensive statistics tracking with session vs lifetime comparisons!

### Features
- **Dual Tracking**: Session statistics vs All-Time records
- **Automatic Tracking**: Distance, speed, G-forces, achievements, science
- **Performance Metrics**: Average speed, exploration rate, landing success rate
- **Records System**: Top 10 records with timestamps
- **Complete Comparison View**: Side-by-side session vs lifetime stats

### Statistics Tracked

#### Flight Statistics
- Total distance traveled
- Maximum speed achieved
- Maximum G-force experienced
- Total flight time

#### Exploration Progress
- Planets visited
- Science experiments completed
- Science points earned
- Waypoints created

#### Achievement Progress
- Achievements unlocked
- Achievement points earned
- Landing attempts vs successes

#### Risk & Survival
- Close calls survived
- Cosmic events experienced
- Closest black hole approach
- Closest sun approach

#### Performance Analytics
- Average speed (distance/time)
- Exploration rate (planets/minute)
- Landing success rate (%)

### How to Use
- **Press F10** to view complete statistics with session vs lifetime comparison
- Statistics automatically update during gameplay
- Records are saved with timestamps
- Color-coded display shows session (left) vs lifetime (right) values

---

## Planet Information Panel

### Overview
Full-screen interactive encyclopedia for every celestial body!

### Features
- **Complete Planetary Data**: Physical properties, atmosphere, composition, temperature
- **Scientific Overview**: Detailed descriptions and fascinating facts
- **Science Progress**: View completed experiments and available studies
- **Professional Layout**: Two-column design with color-coded sections
- **Easy Navigation**: Open with F11 when target locked

### Information Displayed

#### Physical Properties
- Radius and mass
- Surface gravity
- Current position
- Orbital velocity

#### Environmental Data
- Atmospheric composition (if present)
- Surface/core composition
- Temperature ranges
- Habitability information

#### Scientific Data
- Science experiments available
- Completed vs pending experiments
- Potential science points
- Scan status

#### Fascinating Facts
- 3 interesting facts per celestial body
- Historical information
- Unique characteristics
- Astronomical significance

### Available Planets
Complete data for all 10 celestial bodies:
- **Sun** - The heart of our solar system
- **Mercury** - The swift planet
- **Venus** - Earth's twin
- **Earth** - The blue marble
- **Mars** - The red planet
- **Jupiter** - King of the planets
- **Saturn** - Lord of the rings
- **Uranus** - The tilted giant
- **Neptune** - The windy planet
- **Black Hole** - Extreme gravitational phenomenon

### How to Use
1. **Lock a target** in the GUI using the Navigation panel
2. **Press F11** to open the full-screen planet information panel
3. **Read all sections**: Physical data, atmosphere, composition, temperature, facts
4. **Check science status**: See what experiments you've completed
5. **Press ESC or Close button** to return to simulation

### Tips
- Use during long journeys to learn about your destination
- Check science status before scanning to see what's available
- Great for education and immersion
- Full-screen design eliminates distractions

---

## Advanced HUD Overlay

### Overview
Professional heads-up display with targeting reticle, vectors, and indicators!

### HUD Elements

#### Targeting Reticle (Center)
- **Animated crosshair** that pulses when target is locked
- **Corner brackets** for modern tactical feel
- **Center dot** for precise aiming
- **Color-coded**: Cyan normally, pulses when locked

#### Speed Vector Indicator
- **Direction arrow** showing velocity direction
- **Length represents speed**: Longer = faster
- Helps visualize trajectory
- Green color for visibility

#### Distance Ruler (Right Side)
- **Vertical ruler** showing distance to locked target
- **Tick marks** at regular intervals
- **Target name** and exact distance
- Only visible when target is locked

#### Navigation Compass (Bottom Right)
- **Circular compass** with 8 directions
- **N, S, E, W** cardinal directions
- **NE, NW, SE, SW** intercardinal directions
- Helps with spatial orientation

#### Prograde/Retrograde Markers
- **Prograde (Green)**: Direction of velocity
  - Circle with dot and radiating lines
  - KSP-style design
- **Retrograde (Orange)**: Opposite of velocity
  - Circle with X
  - For braking maneuvers

#### Orbit Indicators
- **Radial In/Out**: For orbital adjustments
- Shows gravitational reference frame
- Only visible when target is locked

#### Corner Overlays

**Top Left - Altitude**
- Current altitude above nearest body
- Color-coded warnings:
  - Red < 5 units (DANGER)
  - Yellow < 15 units (CAUTION)
  - Green > 15 units (SAFE)

**Bottom Left - Speed**
- Current speed in m/s
- Large, readable font

**Top Right - G-Force**
- Real-time G-force measurement
- Linked to telemetry system
- Warning when > 5G

### How to Use
- HUD elements automatically appear during flight
- **Target locking** enables distance ruler and orbit indicators
- All elements are non-intrusive and professional
- Hidden when Planet Information Panel is open
- Uses transparent overlays for clarity

### Benefits
- **Situational awareness** - Always know your orientation
- **Precision flying** - Target reticle helps with alignment
- **Safety warnings** - Altitude and G-force indicators
- **Professional feel** - KSP-inspired design
- **Educational** - Learn orbital mechanics concepts

---

## Resource Management System

### Overview
Manage fuel, power, and life support for strategic gameplay!

### Resources Tracked

#### Fuel (1000 units max)
- **Consumption**: 2 units/sec while thrusting, 0.1 units/sec idle
- **Refueling**: Automatic when landing on planets
- **Critical Threshold**: Warning at 20 units
- **Gameplay Impact**: Cannot thrust without sufficient fuel

#### Power (100 units max)
- **Consumption**: 0.5 units/sec for ship systems
- **Regeneration**: Solar panels charge near the sun
- **Solar Efficiency**: Peaks at optimal distance from sun
- **Critical Threshold**: Warning at 15 units
- **Gameplay Impact**: Systems fail without power

#### Oxygen (100 units max)
- **Consumption**: 0.2 units/sec constant
- **Regeneration**: Automatic on planets with atmospheres
- **Breathable Atmospheres**: Earth, Venus, Mars, Jupiter, Saturn, Uranus, Neptune
- **Critical Threshold**: Warning at 25 units
- **Gameplay Impact**: Life support failure

### Features
- **Real-time Monitoring**: HUD bars show all resource levels
- **Color-coded Warnings**: Green (safe), Yellow (caution), Red (critical)
- **Automatic Resupply**: Full refuel/recharge when landing
- **Solar Dynamics**: Power generation scales with distance from sun
- **Strategic Planning**: Plan routes around refueling stations (planets)

### Resource Display
- **HUD Bars** (bottom-center): Live fuel/power/oxygen percentages
- **Ctrl+R**: Display detailed resource status in console

### Tips
- Monitor fuel before long journeys
- Stay near sun for maximum solar power
- Land frequently to maintain resources
- Critical warnings give time to reach safety
- Oxygen regenerates on any planet with atmosphere

---

## Damage & Repair System

### Overview
Ship health and subsystem damage add consequences to risky maneuvers!

### Hull Integrity
- **Max Health**: 100 HP
- **HUD Display**: Health bar at top-center of screen
- **Critical Threshold**: Warning below 25 HP
- **Color Coding**: Green (healthy), Yellow (damaged), Orange (severe), Red (critical)

### Damage Sources

#### Collision Damage
- **Impact velocity**: Higher speed = more damage
- **Cap**: Maximum 50 HP per collision
- **Effect**: Immediate health loss + random subsystem damage

#### G-Force Damage
- **Threshold**: Damage occurs above 10 G
- **Rate**: 0.5 HP per G over 10
- **Effect**: Gradual damage during extreme maneuvers

#### Radiation Damage
- **Source**: Black hole proximity
- **Rate**: Scales with radiation intensity
- **Special**: Extra damage to electronics (sensors, navigation)

#### Solar Damage
- **Source**: Too close to sun (intensity > 100)
- **Rate**: Scales with solar intensity
- **Effect**: Heat-based damage over time

### Subsystem Damage

#### 6 Subsystems Tracked:
1. **Engines**: Reduced thrust when damaged
2. **Sensors**: Science scans disabled
3. **Navigation**: Waypoint system compromised
4. **Life Support**: Faster oxygen drain
5. **Communications**: Delayed achievements
6. **Hull**: Overall structural integrity

#### Damage Severity Levels:
- **None**: 0% damage - Operational
- **Minor**: 1-24% damage - Slight degradation
- **Moderate**: 25-49% damage - Performance degraded
- **Severe**: 50-74% damage - Major malfunction
- **Critical**: 75-100% damage - System failing

### Repair System
- **Automatic Full Repair**: When landing on planets
- **Manual Repair** (future): Repair specific subsystems
- **Damage Report**: Ctrl+D to view all subsystem status

### Gameplay Impact
- **Risk vs Reward**: Aggressive flying damages ship
- **Strategic Landings**: Land to repair before critical missions
- **Subsystem Management**: Plan routes based on functional systems
- **Consequence**: High-speed collisions are punishing

---

## Mission System

### Overview
Structured objectives with rewards and progression!

### Mission Types

#### 1. Exploration Missions
- **Goal**: Visit specific planets or locations
- **Example**: "Visit 3 different planets"
- **Rewards**: Credits, science points, achievements

#### 2. Science Missions
- **Goal**: Complete science experiments
- **Example**: "Complete 5 experiments"
- **Rewards**: Science points, upgrades

#### 3. Speed Run Missions
- **Goal**: Reach locations within time limits
- **Example**: "Reach Neptune in 10 minutes"
- **Challenge**: High difficulty, time pressure

#### 4. Precision Missions
- **Goal**: Execute precise maneuvers
- **Example**: "Land with velocity < 5 m/s"
- **Skill**: Requires careful control

#### 5. Survival Missions
- **Goal**: Survive dangerous conditions
- **Example**: "Survive 3 cosmic events"
- **Challenge**: Tests durability and skill

#### 6. Collection Missions
- **Goal**: Gather multiple items/scans
- **Example**: "Scan all planets"
- **Depth**: Long-term objectives

#### 7. Rescue Missions
- **Goal**: Navigate to specific waypoints
- **Example**: "Reach emergency beacon"
- **Story**: Narrative-driven

### 10 Missions Available

1. **First Steps** (Easy) - Learn basics, earn 100 credits
2. **Planetary Explorer** (Easy) - Visit 3 planets, earn 250 credits
3. **Science Mission** (Medium) - Complete 5 experiments, earn 500 credits
4. **Speed Demon** (Medium) - Reach 100 m/s, earn 400 credits
5. **Precision Landing** (Medium) - Land on Mars safely, earn 600 credits
6. **Grand Tour** (Hard) - Visit all 8 planets, earn 2000 credits
7. **Danger Zone** (Hard) - Survive extreme conditions, earn 1500 credits
8. **Speed Challenge** (Extreme) - Reach Neptune in 10 minutes, earn 3000 credits
9. **Science Master** (Extreme) - Complete all experiments, earn 5000 credits
10. **Perfect Run** (Extreme) - Grand Tour with no damage, earn 10,000 credits

### Mission Features
- **Progress Tracking**: Real-time objective completion
- **Time Limits**: Some missions have countdown timers
- **Rewards**: Credits, science points, achievement points, unlocks
- **Difficulty Scaling**: Easy → Medium → Hard → Extreme
- **Multiple Objectives**: Most missions have 2-3 objectives

### How to Use
- **Ctrl+M**: Display all available missions
- **Mission Panel**: View active mission progress
- **Auto-Complete**: Missions complete when all objectives met
- **Rewards**: Automatically granted on completion

---

## Save/Load System

### Overview
Preserve your progress with full game state saving!

### Save Features

#### What Gets Saved:
- **Ship State**: Position, velocity, orientation (yaw, pitch)
- **Resources**: Fuel, power, oxygen (current and max)
- **Health**: Hull integrity and subsystem damage
- **Statistics**: All session and lifetime stats
- **Game Time**: Total playtime and time scale
- **Target**: Currently locked target

#### Save Format:
- **File Type**: .sav files (JSON-like, human-readable)
- **Location**: `saves/` directory
- **Editable**: Text files you can manually edit
- **Timestamps**: Each save has date/time stamp

### Save Types

1. **Quicksave** (F12)
   - Fast save to `quicksave.sav`
   - Overwrites previous quicksave
   - Perfect for regular saving

2. **Autosave** (automatic)
   - Saves to `autosave.sav`
   - Can be enabled in future updates

3. **Manual Save** (future)
   - Custom filename
   - Multiple save slots

### Load System

1. **Quickload** (Ctrl+L)
   - Loads from `quicksave.sav`
   - Restores complete game state
   - Instant restoration

### File Structure
Save files include:
- Metadata (name, date, version)
- Ship state section
- Resources section
- Health & damage section
- Statistics section
- Game state section

### Tips
- **Save Often**: Use F12 regularly
- **Before Risky Maneuvers**: Quicksave before dangerous attempts
- **Backup Important Saves**: Copy .sav files for safety
- **Edit Saves**: Advanced users can manually edit save files

---

## Audio System

### Overview
Immersive sound effects and music enhance gameplay atmosphere!

### Sound Effects

#### Flight Sounds
- **Engine Thrust**: Looping sound when thrusting
- **Engine Stop**: When thrust ends
- **Landing Success**: Successful landing confirmation
- **Landing Failure**: Crash landing warning
- **Takeoff**: Leaving planet surface

#### Achievement Sounds
- **Achievement Unlock**: Satisfying notification
- **Science Scan Start**: Scan initiation
- **Science Scan Complete**: Experiment finished
- **Waypoint Create**: Marker placed

#### Warning Sounds
- **Collision Warning**: Imminent impact alert
- **Damage Taken**: Ship hit
- **Critical Warning**: System failures
- **Cosmic Event**: Space phenomena alert

#### System Sounds
- **UI Click**: Button confirmation
- **UI Hover**: Menu navigation
- **Refuel**: Resource resupply
- **Repair Complete**: Damage fixed
- **Pause/Unpause**: Time control

### Music Tracks (Placeholder)
- **Main Theme**: Title music
- **Space Ambient**: Calm exploration
- **Intense Flight**: High-speed action
- **Near Black Hole**: Ominous atmosphere
- **Planet Approach**: Arrival music
- **Victory**: Mission complete

### Audio Controls
- **Ctrl+A**: Display audio status
- **Volume Controls**: Master, SFX, Music (future GUI)
- **Toggle Audio**: Enable/disable (future hotkey)
- **Placeholder Mode**: System ready for real audio files

### Adding Custom Sounds
1. Create `sounds/` directory
2. Add .wav or .ogg files with proper names
3. System will auto-load them
4. Supported formats: WAV, OGG via SFML Audio

### Notes
- Audio system is fully functional
- Currently in placeholder mode (no actual sound files)
- Ready for real audio files to be added
- Zero-impact on performance when sounds aren't loaded

---

## Enhanced Console Logging

### Overview
Professional, color-coded console output for better readability!

### Features
- **Timestamps** on all messages
- **Log levels** with color coding
- **Boxed messages** for important events
- **Progress bars** for operations
- **Section headers** for organization

### Log Levels
- **INFO** (Blue) - General information
- **SUCCESS** (Green) - Successful operations
- **WARNING** (Yellow) - Caution messages
- **ERROR** (Red) - Error conditions
- **DEBUG** (Magenta) - Debug information
- **SYSTEM** (Cyan) - System messages
- **SCIENCE** (Green) - Science data
- **ACHIEVEMENT** (Bright Cyan) - Achievement unlocks

### Special Formatting
- Boxed achievement unlocks
- Section headers for categories
- Progress bars for scans
- Table formatting for data
- Color-coded severity levels

### How to Use
- All systems automatically use enhanced logging
- **Press F9** for logging demo
- Color-coded output improves information scan
- Timestamps help track event sequences

---

## Keyboard Shortcuts Reference

### Basic Controls
| Key | Function |
|-----|----------|
| I / ↑ | Forward thrust |
| K / ↓ | Reverse thrust |
| J / ← | Rotate left |
| L / → | Rotate right |
| U | Pitch up |
| O | Pitch down |
| C | Cycle camera mode |
| P | Pause/Resume |
| +/= | Speed up time |
| -/_ | Slow down time |
| ESC | Exit |

### Display Toggles
| Key | Function |
|-----|----------|
| F1 | Toggle Help Overlay (ImGui) |
| Space | Toggle velocity vectors |
| T | Toggle trajectory markers |
| R | Toggle ship trajectory |
| G | Toggle coordinate grid |
| H | Calculate Hohmann transfer |
| L | Toggle Lagrange points |
| M | Show orbital mechanics |

### Advanced System Hotkeys
| Key | Function |
|-----|----------|
| **F2** | Achievement Summary |
| **F3** | Science Data Summary |
| **F4** | Advanced Telemetry Display |
| **F5** | List All Waypoints |
| **F6** | Create Waypoint Here |
| **F7** | Start Science Scan |
| **F8** | Trigger Random Cosmic Event |
| **F9** | Enhanced Logger Demo |
| **F10** | Complete Statistics (Session vs Lifetime) |
| **F11** | Planet Information Panel (Full-Screen) |
| **F12** | Quick Save (Save current game state) |
| **Ctrl+L** | Quick Load (Load last quicksave) |
| **Ctrl+R** | Display Resource Status (Fuel/Power/Oxygen) |
| **Ctrl+D** | Display Damage Report (Hull/Subsystems) |
| **Ctrl+M** | Display Mission List |
| **Ctrl+A** | Display Audio Status |

### Landing Controls
| Key | Function |
|-----|----------|
| B | Attempt landing |
| N | Take off |

---

## Tips & Strategies

### Achievement Hunting
1. **Speed runs**: Find open space and accelerate to max speed
2. **Exploration**: Visit planets in order of distance
3. **Survival**: Practice close approaches in safe areas first
4. **Black hole approach**: Approach slowly, watch altitude warnings

### Science Maximization
1. **Plan routes**: Visit all planets systematically
2. **High-value targets**: Save Sun/Black Hole for last (risky!)
3. **Optimal distance**: Get as close as safe (< 50 units)
4. **Multiple scans**: Each planet scans once, choose carefully

### Navigation Tips
1. **Use waypoints**: Mark important locations early
2. **Safe zones**: Create waypoints at safe observation points
3. **Return markers**: Mark fuel stations and landing sites
4. **Route planning**: Chain waypoints for efficient travel

### Telemetry Monitoring
1. **G-force management**: Keep below 8G for safety
2. **Altitude awareness**: Watch red/yellow warnings
3. **Orbital analysis**: Use radial/tangential velocity for orbit insertion
4. **Performance tracking**: Monitor max speeds and distances

### Event Handling
1. **Solar flares**: Increase distance from Sun during events
2. **Meteor showers**: Reduce speed, improve maneuverability
3. **Gravitational anomalies**: Trust instruments, not intuition
4. **Stellar phenomena**: Perfect time for science scans!

### Combo Strategies
1. **Science + Achievements**: Close approaches earn both
2. **Waypoints + Navigation**: Mark targets before long journeys
3. **Telemetry + Landing**: Use G-force data for soft landings
4. **Events + Opportunities**: Some events boost science value

---

## Quick Start Guide

### First 5 Minutes
1. **Press F1** - Read the help overlay
2. **Press F2** - Check available achievements
3. **Lock a target** - Use GUI Navigation panel
4. **Press F11** - View full planet information
5. **Press F7** - Start your first science scan!
6. **Press F12** - Save your first quicksave!

### First Hour Goals
- [ ] Unlock "Space Cadet" (20 m/s)
- [ ] Scan 3 planets and view their info panels
- [ ] Create 5 waypoints
- [ ] Experience a cosmic event
- [ ] Check telemetry at max speed
- [ ] View Advanced HUD resource bars (fuel/power/oxygen)
- [ ] Complete your first mission (Ctrl+M to view)
- [ ] Land on a planet to refuel and repair
- [ ] Check F10 statistics comparison

### Advanced Goals
- [ ] Complete Grand Tour (all 8 planets)
- [ ] Approach black hole safely
- [ ] Collect 500+ science points
- [ ] Earn 1000+ achievement points
- [ ] Survive 8+ G-forces

---

## Statistics Tracking

### View Anytime
- **F2**: Achievement points, unlocks, max speed
- **F3**: Science points, experiments completed
- **F4**: Current telemetry, G-forces, orbital data
- **F5**: Waypoint count and positions
- **F10**: Enhanced statistics with session vs lifetime comparison
- **F11**: Full-screen planet information (when target locked)

### Automatic Tracking
- Distance traveled
- Max speed achieved
- Planets visited
- Close calls survived
- Landing attempts
- Science experiments
- G-force records

---

## System Integration

All new systems are fully integrated:
- **GUI panels** show real-time data
- **Advanced HUD** provides professional overlay with targeting reticle
- **Console logging** provides detailed feedback
- **Achievements** unlock based on gameplay
- **Science** tracks exploration progress
- **Waypoints** assist navigation
- **Telemetry** monitors performance
- **Events** add dynamic challenges
- **Statistics** compare session vs lifetime performance
- **Planet Panel** displays full-screen celestial body information

**Press F1 in-game for interactive help!**
**Press F10 for session vs lifetime statistics!**
**Press F11 for full-screen planet information!**

---

## Feature Highlights

### Why These Features Matter

**Achievements** → Gamification, replayability, goals
**Science** → Meaningful exploration, progression system
**Waypoints** → Navigation aid, player freedom
**Telemetry** → Realism, precision flying, education
**Events** → Dynamic gameplay, unpredictability
**Statistics** → Progress tracking, session comparison, motivation
**Planet Info** → Educational encyclopedia, immersion, context
**Advanced HUD** → Professional UI, situational awareness, precision
**Logging** → Professional feel, better feedback

### Educational Value
- Real orbital mechanics (Hohmann transfers)
- G-force effects on pilots
- Scientific data collection methodology
- Navigation and waypoint systems
- Space hazards and safety

### Gameplay Depth
- Multiple progression systems
- Risk vs reward decisions
- Strategic route planning
- Skill-based challenges
- Long-term goals

---

**🚀 Ready to explore? Launch the simulator and press F1 to begin!**

*For questions or feedback, check the main README or project documentation.*
