Orbital Engine Simulation

A custom 3D space simulation engine built from scratch in modern C++ using OpenGL.

Tech Stack

C++17

OpenGL 4.x (Core Profile)

GLAD

SFML (window + context management)

Custom math library (Vec3, Mat4)

GLSL shader pipeline

Overview

This project is a from-scratch 3D engine designed to render and simulate a space environment.
It uses a fully modern OpenGL pipeline (no deprecated fixed-function code) and is structured with a clean, modular architecture inspired by real engine design.

The goal is to build a scalable foundation for:

Planet rendering (sphere meshes)

Star fields

Camera navigation in 3D space

Spaceships and interactive objects

Newtonian gravity and orbital motion

All systems are separated into dedicated modules (rendering, physics, math, universe, entities) rather than placing everything inside main.cpp.

Engine Architecture

math/ → Custom vector, matrix, and camera systems

rendering/ → Shader management, mesh handling, renderer

physics/ → Gravity and motion simulation

universe/ → Planet and solar system generation

entities/ → Ships and interactive objects

effects/ → Starfield rendering
