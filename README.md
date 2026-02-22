A custom 3D space simulation engine built in modern C++ using OpenGL.

          Tech Stack

C++17

OpenGL 4.x (Core Profile)

GLAD (OpenGL loader)

SFML (window & OpenGL context management)

Custom math library (Vec3, Mat4)

Custom shader pipeline (GLSL)

          What This Project Is

This project is a from-scratch 3D engine focused on rendering and simulating a space environment. It uses a fully modern OpenGL pipeline (no deprecated fixed-function code) and is structured with clean engine-style architecture.

The goal is to simulate:

3D planets (sphere meshes)

Camera navigation in space

Spaceships

Star fields

Newtonian gravity and orbital motion

Everything is modular and separated into systems (rendering, physics, math, universe, etc.) instead of putting everything in main.cpp.

          Engine Structure

math/ → Custom vectors, matrices, camera system

rendering/ → Shader system, mesh handling, renderer

physics/ → Gravity and motion simulation

universe/ → Planets and solar system creation

entities/ → Spaceship and interactive objects

effects/ → Star field rendering

         Current Status

OpenGL pipeline fully working

Depth testing enabled

3D cube rendering confirmed

Custom camera system implemented

Projection and view matrices working

Next steps:

Sphere mesh generation

Planet rendering

Starfield background

Gravity simulation

Spaceship controls

This is an ongoing engine project focused on low-level graphics programming, physics simulation, and clean system architecture.
