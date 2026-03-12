cmake_minimum_required(VERSION 3.15)
project(BlackHoleTravelSimulator VERSION 2.0 LANGUAGES CXX)

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

# SFML - adjust path as needed
set(SFML_DIR "C:/Dev/SFML-2.6.2/lib/cmake/SFML")
find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)
# Note: If version mismatch occurs, remove version number or adjust SFML path

# GLAD - adjust path as needed
set(GLAD_INCLUDE_DIR "C:/Dev/glad/include")
set(GLAD_SOURCE "C:/Users/amjad/Downloads/glad/src/glad.c")

# ImGui - OPTIONAL - uncomment after installing ImGui
# Download from: https://github.com/ocornut/imgui
# set(IMGUI_DIR "C:/Dev/imgui")
# set(IMGUI_SFML_DIR "C:/Dev/imgui-sfml")

# ImGui source files - OPTIONAL
# Uncomment these after installing ImGui
# set(IMGUI_SOURCES
#     ${IMGUI_DIR}/imgui.cpp
#     ${IMGUI_DIR}/imgui_draw.cpp
#     ${IMGUI_DIR}/imgui_tables.cpp
#     ${IMGUI_DIR}/imgui_widgets.cpp
#     ${IMGUI_SFML_DIR}/imgui-SFML.cpp
# )

# For now, ImGui is disabled - using stub version
set(IMGUI_SOURCES "")

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
    Mesh.hpp
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
    # Uncomment these after installing ImGui:
    # ${IMGUI_DIR}
    # ${IMGUI_SFML_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    OpenGL::GL
    sfml-graphics
    sfml-window
    sfml-system
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
message(STATUS "SFML Version: ${SFML_VERSION}")
message(STATUS "=================================================")
message(STATUS "")
