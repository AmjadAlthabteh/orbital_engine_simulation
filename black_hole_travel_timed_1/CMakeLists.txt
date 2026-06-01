cmake_minimum_required(VERSION 3.15)
project(BlackHoleTravelSimulator VERSION 2.0 LANGUAGES C CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for clangd/tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler-specific flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
        -Wno-missing-field-initializers
    )

    # Debug flags
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0)
    endif()

    # Release flags
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -march=native)
    endif()
endif()

# Enable sanitizers (optional - uncomment to use)
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(ENABLE_UBSAN "Enable Undefined Behavior Sanitizer" OFF)
option(ENABLE_TSAN "Enable Thread Sanitizer" OFF)

if(ENABLE_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
    message(STATUS "Address Sanitizer ENABLED")
endif()

if(ENABLE_UBSAN)
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
    message(STATUS "Undefined Behavior Sanitizer ENABLED")
endif()

if(ENABLE_TSAN)
    add_compile_options(-fsanitize=thread)
    add_link_options(-fsanitize=thread)
    message(STATUS "Thread Sanitizer ENABLED")
endif()

# Find packages
find_package(OpenGL REQUIRED)

# Dependency paths (configure via -D...=...)
# These defaults intentionally prefer local "external/" folders when present.
set(SFML_DIR "" CACHE PATH "Path to SFMLConfig.cmake folder (e.g. .../lib/cmake/SFML)")
set(GLAD_INCLUDE_DIR "" CACHE PATH "Path to glad include dir (contains glad/glad.h)")
set(GLAD_SOURCE "" CACHE FILEPATH "Path to glad.c source file")
set(IMGUI_DIR "" CACHE PATH "Path to Dear ImGui root (contains imgui.cpp)")
set(IMGUI_SFML_DIR "" CACHE PATH "Path to imgui-sfml root (contains imgui-SFML.cpp)")

if(NOT SFML_DIR AND EXISTS "C:/Dev/SFML-2.6.2/lib/cmake/SFML")
    set(SFML_DIR "C:/Dev/SFML-2.6.2/lib/cmake/SFML")
endif()
if(NOT IMGUI_DIR AND EXISTS "C:/Dev/imgui/imgui.cpp")
    set(IMGUI_DIR "C:/Dev/imgui")
endif()
if(NOT IMGUI_SFML_DIR AND EXISTS "C:/Dev/imgui-sfml/imgui-SFML.cpp")
    set(IMGUI_SFML_DIR "C:/Dev/imgui-sfml")
endif()
if(NOT GLAD_INCLUDE_DIR AND EXISTS "C:/Dev/glad/include/glad/glad.h")
    set(GLAD_INCLUDE_DIR "C:/Dev/glad/include")
endif()
if(NOT GLAD_SOURCE AND EXISTS "C:/Users/amjad/Downloads/glad/src/glad.c")
    set(GLAD_SOURCE "C:/Users/amjad/Downloads/glad/src/glad.c")
endif()

if(NOT SFML_DIR AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SFML/lib/cmake/SFML")
    set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/SFML/lib/cmake/SFML")
endif()
if(NOT IMGUI_DIR AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui/imgui.cpp")
    set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")
endif()
if(NOT IMGUI_SFML_DIR AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml/imgui-SFML.cpp")
    set(IMGUI_SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-sfml")
endif()
if(NOT GLAD_INCLUDE_DIR AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include/glad/glad.h")
    set(GLAD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include")
endif()
if(NOT GLAD_SOURCE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/src/glad.c")
    set(GLAD_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/external/glad/src/glad.c")
endif()

if(NOT IMGUI_DIR OR NOT EXISTS "${IMGUI_DIR}/imgui.cpp")
    message(FATAL_ERROR "ImGui not found. Set -DIMGUI_DIR=... (must contain imgui.cpp).")
endif()
if(NOT IMGUI_SFML_DIR OR NOT EXISTS "${IMGUI_SFML_DIR}/imgui-SFML.cpp")
    message(FATAL_ERROR "ImGui-SFML not found. Set -DIMGUI_SFML_DIR=... (must contain imgui-SFML.cpp).")
endif()
if(NOT GLAD_INCLUDE_DIR OR NOT EXISTS "${GLAD_INCLUDE_DIR}/glad/glad.h")
    message(FATAL_ERROR "GLAD headers not found. Set -DGLAD_INCLUDE_DIR=... (must contain glad/glad.h).")
endif()
if(NOT GLAD_SOURCE OR NOT EXISTS "${GLAD_SOURCE}")
    message(FATAL_ERROR "GLAD source not found. Set -DGLAD_SOURCE=... (path to glad.c).")
endif()

find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)
# Note: If version mismatch occurs, remove version number or adjust SFML path

# ImGui source files
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_SFML_DIR}/imgui-SFML.cpp
)

# Source files
set(SOURCES
    main.cpp
    Body.cpp
    Camera.cpp
    CelestialBody.cpp
    CollisionPredictor.cpp
    CoordinateGrid.cpp
    EngineParticles.cpp
    Entity.cpp
    GalaxyBackground.cpp
    HohmannTransfer.cpp
    LagrangePoints.cpp
    NebulaBackground.cpp
    OrbitalMechanicsHUD.cpp
    OrbitalPath.cpp
    PhysicsEngine.cpp
    PlanetaryRings.cpp
    PredictionMarker.cpp
    Renderer.cpp
    Shader.cpp
    SolarSystemFactory.cpp
    Spaceship.cpp
    Sphere.cpp
    StarField.cpp
    StoryNarrator.cpp
    TextureLoader.cpp
    Trail.cpp
    VectorRenderer.cpp
    ${GLAD_SOURCE}
    ${IMGUI_SOURCES}
)

# Header files (for IDEs)
set(HEADERS
    Body.hpp
    Camera.hpp
    CelestialBody.hpp
    CollisionPredictor.hpp
    CoordinateGrid.hpp
    EngineParticles.hpp
    EnhancedShaders.hpp
    Entity.hpp
    GalaxyBackground.hpp
    HohmannTransfer.hpp
    HUD.hpp
    LagrangePoints.hpp
    Mat4.hpp
    NebulaBackground.hpp
    OrbitalMechanicsHUD.hpp
    OrbitalPath.hpp
    PhysicsEngine.hpp
    PlanetaryRings.hpp
    PredictionMarker.hpp
    Renderer.hpp
    Shader.hpp
    SolarSystemFactory.hpp
    Spaceship.hpp
    Sphere.hpp
    StarField.hpp
    StoryNarrator.hpp
    TextureLoader.hpp
    Trail.hpp
    Vec3.hpp
    VectorRenderer.hpp
    Framebuffer.hpp
    PostProcessing.hpp
    BlackHole.hpp
    AccretionDisk.hpp
    GUI.hpp
    GUI_NoImGui.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLAD_INCLUDE_DIR}
    ${SFML_INCLUDE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_SFML_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE gdi32 user32 kernel32)
endif()

# Install target
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# Print configuration
message(STATUS "")
message(STATUS "=== Black Hole Travel Simulator Configuration ===")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
if(DEFINED SFML_VERSION)
    message(STATUS "SFML Version: ${SFML_VERSION}")
endif()
message(STATUS "=================================================")
message(STATUS "")
