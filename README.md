# Orbital Engine Simulation

A real-time 3D orbital simulation written in C++17 with OpenGL, SFML, GLAD,
Dear ImGui, and a custom math and physics stack.

## Features

- Newtonian gravity and orbital motion
- Procedural planets, stars, nebulae, and black-hole effects
- Spaceship navigation, trajectories, waypoints, and collision prediction
- Interactive telemetry and orbital-mechanics HUDs
- Configurable post-processing, particles, audio, missions, and achievements

## Quick start

Requirements:

- CMake 3.15 or newer
- A C++17 compiler
- OpenGL
- SFML 2.6
- Dear ImGui and ImGui-SFML

On Windows, open `black_hole_travel_timed_1.sln` in Visual Studio, or run:

```bat
cd black_hole_travel_timed_1
build.bat --release --run
```

For dependency path options and other build methods, see
[`BUILD_AND_TEST.md`](BUILD_AND_TEST.md).

## Run checks

Before sharing changes, build the Visual Studio solution or run the provided
script for the configuration you changed.

## Project structure

The simulation is split into focused modules for rendering, physics, camera
control, celestial bodies, spacecraft, visual effects, and UI systems. The
main implementation is in [`black_hole_travel_timed_1/`](black_hole_travel_timed_1/).

## Contributing and security

Bug reports and focused pull requests are welcome. Please report security
issues according to [`SECURITY.md`](SECURITY.md).

Project site: <https://www.orbitalcollision.dev/>
