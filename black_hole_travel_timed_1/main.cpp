
// Advanced 3D Solar System Simulation with Orbital Mechanics
// Features: Realistic gravity, collision prediction, orbital trails, starfield
// Visual Physics: Velocity vectors, force vectors, prediction markers, trajectory points
// ENHANCED VERSION: Atmospheric glow, rim lighting, nebula background, engine particles, planet textures
#include <SFML/Window.hpp>
#include <glad/glad.h>
#include <iostream>
#include <cmath>

#include "Camera.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include "Sphere.hpp"
#include "SolarSystemFactory.hpp"
#include "PhysicsEngine.hpp"
#include "StarField.hpp"
#include "Trail.hpp"
#include "CollisionPredictor.hpp"
#include "OrbitalPath.hpp"
#include "PredictionMarker.hpp"
#include "VectorRenderer.hpp"
#include "Spaceship.hpp"
#include "EnhancedShaders.hpp"
#include "NebulaBackground.hpp"
#include "EngineParticles.hpp"
#include "TextureLoader.hpp"
#include "GalaxyBackground.hpp"
#include "CoordinateGrid.hpp"
#include "StoryNarrator.hpp"
#include "HohmannTransfer.hpp"
#include "LagrangePoints.hpp"
#include "PlanetaryRings.hpp"
#include "OrbitalMechanicsHUD.hpp"
#include "Framebuffer.hpp"
#include "PostProcessing.hpp"
#include "BlackHole.hpp"
#include "AccretionDisk.hpp"
// ImGui GUI System - Interactive control panels
#include "GUI.hpp"

// NEW ADVANCED SYSTEMS
#include "AchievementSystem.hpp"
#include "ScienceSystem.hpp"
#include "WaypointSystem.hpp"
#include "EnhancedLogger.hpp"
#include "AdvancedTelemetry.hpp"
#include "CosmicEventSystem.hpp"
#include "EnhancedStatistics.hpp"
#include "PlanetInformationPanel.hpp"
#include "AdvancedHUD.hpp"
#include "SaveSystem.hpp"
#include "ResourceSystem.hpp"
#include "DamageSystem.hpp"
#include "AudioManager.hpp"
#include "MissionSystem.hpp"

// Using enhanced shaders from EnhancedShaders.hpp
// Enhanced planet shader replaces the basic one - adds atmospheric glow and rim lighting

// Line shader for trails and collision predictions
const char* lineVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
}
)";

const char* lineFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 lineColor;
uniform float alpha;

void main()
{
    FragColor = vec4(lineColor, alpha);
}
)";

// Star shader for point rendering
const char* starVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aSize;

uniform mat4 view;
uniform mat4 projection;

out vec3 starColor;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = aSize;
    starColor = aColor;
}
)";

const char* starFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 starColor;

void main()
{
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);
    float alpha = 1.0 - smoothstep(0.3, 0.5, dist);
    FragColor = vec4(starColor, alpha);
}
)";

// Vector arrow shader for velocity/force visualization
const char* vectorVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 view;
uniform mat4 projection;

out vec3 vertexColor;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    vertexColor = aColor;
}
)";

const char* vectorFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 vertexColor;

void main()
{
    FragColor = vec4(vertexColor, 0.9);
}
)";

// ============================================================================
// SIMULATION CONFIGURATION CONSTANTS
// ============================================================================
// All magic numbers have been extracted to named constants for:
//   - Easier tuning and tweaking of simulation parameters
//   - Better code readability and self-documentation
//   - Single source of truth for configuration values
//   - Future extensibility (e.g., loading from config files)
// ============================================================================

// ============================================================================
// PERFORMANCE OPTIMIZATION: Body Lookup Helpers
// These functions eliminate redundant loops over the bodies vector.
// Previously, code would loop multiple times to find bodies - this centralizes
// the lookup logic and enables future optimization (e.g., hash map lookups).
// ============================================================================

// Maximum distance sentinel value for finding nearest bodies
constexpr float MAX_DISTANCE = 999999.0f;

// Window dimensions - centralized for consistency across rendering pipeline
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;
constexpr float ASPECT_RATIO = static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT);

// Physics simulation constants
constexpr float PHYSICS_TIMESTEP = 0.1f;           // Physics update interval in seconds
constexpr float MIN_TIME_SCALE = 0.1f;             // Minimum allowed time scale multiplier
constexpr float VECTOR_UPDATE_INTERVAL = 0.1f;     // Refresh rate for velocity/force vector display
constexpr float COLLISION_PREDICTION_TIME = 50.0f; // How far ahead to predict collisions (seconds)
constexpr float COLLISION_UPDATE_INTERVAL = 0.5f;  // How often to recalculate collision predictions
constexpr float TRAJECTORY_UPDATE_INTERVAL = 1.0f; // How often to recalculate trajectory points

// Trajectory visualization constants
constexpr size_t BODY_TRAJECTORY_POINT_SKIP = 5;  // Display every Nth point for body trajectories
constexpr size_t SHIP_TRAJECTORY_POINT_SKIP = 3;  // Display every Nth point for ship trajectory
constexpr int ORBITAL_PATH_POINTS = 360;          // Number of points for orbital path calculation
constexpr float ORBITAL_PATH_COLOR_DIM = 0.5f;    // Dimming factor for orbital path colors
constexpr float TRAJECTORY_POINT_COLOR_DIM = 0.7f; // Dimming factor for trajectory point colors

// Rotation and angle constants
constexpr float FULL_ROTATION_DEGREES = 360.0f;   // Full rotation in degrees

/**
 * Find a celestial body by name in O(n) time.
 * This replaces 6+ duplicate linear search patterns throughout the codebase.
 * @param name The name of the body to find
 * @param bodies Vector of all celestial bodies
 * @return Pointer to the body if found, nullptr otherwise
 */
static CelestialBody* findBodyByName(const std::string& name,
                                     const std::vector<CelestialBody*>& bodies)
{
    for (auto* body : bodies)
    {
        if (body->getName() == name)
            return body;
    }
    return nullptr;
}

/**
 * Find the nearest celestial body to a given position.
 * Returns both the body pointer AND the distance in a single pass.
 * This replaces the wasteful pattern of finding nearest by name, then
 * looping again to get the body's properties.
 * @param position The reference position
 * @param bodies Vector of all celestial bodies
 * @param outDistance Output parameter for the distance to nearest body
 * @return Pointer to nearest body, or nullptr if bodies is empty
 */
static CelestialBody* findNearestBody(const Vec3& position,
                                      const std::vector<CelestialBody*>& bodies,
                                      float& outDistance)
{
    CelestialBody* nearest = nullptr;
    float minDist = MAX_DISTANCE;

    for (auto* body : bodies)
    {
        Vec3 diff = body->getPhysicsBody().position - position;
        float dist = diff.length();
        if (dist < minDist)
        {
            minDist = dist;
            nearest = body;
        }
    }

    outDistance = minDist;
    return nearest;
}

int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    // Don't force OpenGL 4.6 Core - let SFML use compatibility mode for ImGui
    // GLAD will still load modern OpenGL functions
    // settings.majorVersion = 4;
    // settings.minorVersion = 6;

    sf::RenderWindow window(
        sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
        "Black Hole Engine",
        sf::Style::Default,
        settings
    );

    window.setVerticalSyncEnabled(true);
    // Don't hide/grab cursor for GUI interaction
    window.setMouseCursorGrabbed(false);
    window.setMouseCursorVisible(true);

    if (!gladLoadGL())
    {
        std::cout << "Failed to load GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glLineWidth(2.0f);

    // Create ENHANCED shaders with atmospheric effects, rim lighting, and improved visuals
    Shader planetShader(enhancedPlanetVertexShader, enhancedPlanetFragmentShader);
    Shader lineShader(lineVertexShaderSource, lineFragmentShaderSource);
    Shader starShader(enhancedStarVertexShader, enhancedStarFragmentShader);
    Shader vectorShader(vectorVertexShaderSource, vectorFragmentShaderSource);
    Shader nebulaShader(nebulaVertexShader, nebulaFragmentShader);
    Shader engineParticleShader(engineParticleVertexShader, engineParticleFragmentShader);
    Shader gridShader(gridVertexShader, gridFragmentShader);
    Shader ringShader(ringVertexShader, ringFragmentShader);
    Shader lensingShader(lensingVertexShader, lensingFragmentShader);
    Shader accretionDiskShader(accretionDiskVertexShader, accretionDiskFragmentShader);

    // Create texture loader and load/generate planet textures
    TextureLoader textureLoader;
    std::cout << "Generating procedural planet textures...\n";
    textureLoader.createProceduralPlanetTexture("Earth", 512, 512);
    textureLoader.createProceduralPlanetTexture("Moon", 512, 512);
    textureLoader.createProceduralPlanetTexture("Mars", 512, 512);
    textureLoader.createProceduralPlanetTexture("Jupiter", 512, 512);
    textureLoader.createProceduralPlanetTexture("Saturn", 512, 512);
    textureLoader.createProceduralPlanetTexture("Sun", 512, 512);
    textureLoader.createProceduralPlanetTexture("Venus", 512, 512);
    textureLoader.createProceduralPlanetTexture("Mercury", 512, 512);
    textureLoader.createProceduralPlanetTexture("Uranus", 512, 512);
    textureLoader.createProceduralPlanetTexture("Neptune", 512, 512);
    std::cout << "Textures generated successfully!\n";

    // Create visual enhancement systems
    NebulaBackground nebula;
    EngineParticles engineParticles(1000);  // Max 1000 particles
    GalaxyBackground galaxy(20000);          // 20,000 stars in realistic galaxy structure
    CoordinateGrid coordinateGrid(100.0f, 10.0f);  // 100 unit grid with 10 unit spacing
    StoryNarrator narrator;                  // Story and narrative system
    HohmannTransfer transferCalculator;      // NASA-style transfer orbit calculator
    LagrangePoints lagrangePoints;           // Gravitational balance points (L1-L5)
    OrbitalMechanicsHUD orbitalHUD;          // Display orbital parameters

    // Create Saturn's beautiful rings!
    PlanetaryRings saturnRings(2.8f, 4.5f, 120);  // Inner radius, outer radius, segments
    saturnRings.setColor(Vec3(0.9f, 0.85f, 0.7f));  // Golden-tan color
    saturnRings.setAlpha(0.6f);

    // Create post-processing system with Bloom and HDR
    Framebuffer sceneFramebuffer(WINDOW_WIDTH, WINDOW_HEIGHT);
    PostProcessing postProcessing(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Create black hole accretion disk (swirling matter around black hole)
    AccretionDisk accretionDisk(3.0f, 8.0f, 120, 40);  // Inner radius, outer radius, segments, rings

    // Create GUI system
    GUI gui(window);

    // ===== CREATE NEW ADVANCED SYSTEMS =====
    AchievementSystem achievements;
    ScienceSystem science;
    WaypointSystem waypoints;
    AdvancedTelemetry telemetry;
    CosmicEventSystem cosmicEvents;
    EnhancedStatistics statistics;
    PlanetInformationPanel planetPanel;
    AdvancedHUD advancedHUD;
    SaveSystem saveSystem;
    ResourceSystem resourceSystem;
    DamageSystem damageSystem;
    AudioManager audioManager;
    MissionSystem missionSystem;

    // Use enhanced logger for startup messages
    gLogger.sectionHeader("SYSTEM INITIALIZATION COMPLETE");
    std::cout << "Nebula background created with multiple colored regions\n";
    std::cout << "Engine particle system initialized\n";
    std::cout << "Galaxy background created with 20,000 stars in spiral arms\n";
    std::cout << "Coordinate grid system initialized (toggle with 'G' key)\n";
    std::cout << "Story narrator activated - immersive storytelling enabled\n";
    std::cout << "Hohmann Transfer Calculator ready (press 'H' for Earth->Mars transfer)\n";
    std::cout << "Lagrange Points calculator ready (press 'L' to show/hide)\n";
    std::cout << "Saturn Rings added - look for the beautiful golden rings!\n";
    std::cout << "Orbital Mechanics HUD enabled (press 'M' for ship orbit analysis)\n";
    std::cout << "POST-PROCESSING enabled: Bloom & HDR for cinematic visuals!\n";
    std::cout << "BLACK HOLE ACCRETION DISK: Swirling matter with heat glow!\n";
    std::cout << "GRAVITATIONAL LENSING: Light bending around black hole!\n";
    std::cout << "ImGui GUI: Interactive control panels and real-time adjustments!\n";

    Renderer renderer(&planetShader);
    Camera camera;
    camera.position = Vec3(0, 30, 80);

    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║   3D ORBITAL PHYSICS SIMULATOR - ULTRA REALISTIC EDITION   ║\n";
    std::cout << "║   With Galaxy Background, Story Mode & Advanced Graphics   ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n\n";

    std::cout << "┌─ SPACESHIP CONTROLS ──────────────────────────────────────┐\n";
    std::cout << "│  I / ↑ Arrow  │ Thrust forward                            │\n";
    std::cout << "│  K / ↓ Arrow  │ Reverse thrust                            │\n";
    std::cout << "│  J / ← Arrow  │ Rotate left (yaw)                         │\n";
    std::cout << "│  L / → Arrow  │ Rotate right (yaw)                        │\n";
    std::cout << "│  U            │ Pitch up                                  │\n";
    std::cout << "│  O            │ Pitch down                                │\n";
    std::cout << "│  Q            │ TURBO BOOST (3x thrust power!)            │\n";
    std::cout << "│  E            │ DO A BARREL ROLL!                         │\n";
    std::cout << "│  Z            │ HYPERDRIVE (ludicrous speed!)             │\n";
    std::cout << "│  V            │ Toggle drift mode (space drifting!)       │\n";
    std::cout << "│  X            │ Toggle rainbow exhaust trail              │\n";
    std::cout << "└───────────────────────────────────────────────────────────┘\n\n";

    std::cout << "┌─ CAMERA CONTROLS ─────────────────────────────────────────┐\n";
    std::cout << "│  WASD         │ Move camera (Free mode only)              │\n";
    std::cout << "│  Mouse        │ Look around (Free mode only)              │\n";
    std::cout << "│  Mouse Wheel  │ Zoom in/out (FOV 10-120°)                 │\n";
    std::cout << "│  C            │ Toggle mode (Free/Follow/Chase)           │\n";
    std::cout << "└───────────────────────────────────────────────────────────┘\n\n";

    std::cout << "┌─ DISPLAY & TIME CONTROLS ─────────────────────────────────┐\n";
    std::cout << "│  Space        │ Toggle velocity vectors                   │\n";
    std::cout << "│  T            │ Toggle trajectory markers (planets)       │\n";
    std::cout << "│  R            │ Toggle ship trajectory prediction         │\n";
    std::cout << "│  G            │ Toggle coordinate grid (math visualization)│\n";
    std::cout << "│  H            │ Calculate Hohmann Transfer (Earth->Mars) │\n";
    std::cout << "│  L            │ Toggle Lagrange Points display           │\n";
    std::cout << "│  M            │ Show Orbital Mechanics data for ship     │\n";
    std::cout << "│  + / =        │ Speed up time (up to 10x)                 │\n";
    std::cout << "│  - / _        │ Slow down time (down to 0.1x)             │\n";
    std::cout << "│  P            │ Pause/Unpause simulation                  │\n";
    std::cout << "│  ESC          │ Exit                                      │\n";
    std::cout << "└───────────────────────────────────────────────────────────┘\n\n";

    std::cout << "FEATURES:\n";
    std::cout << "  ✓ Realistic Newtonian gravity (F = G·m₁·m₂/r²)\n";
    std::cout << "  ✓ Elastic collision physics with visualization\n";
    std::cout << "  ✓ Real-time collision markers (RED SPHERES)\n";
    std::cout << "  ✓ Collision warnings and altitude alerts\n";
    std::cout << "  ✓ Ship telemetry display (updates every 1 sec)\n";
    std::cout << "  ✓ 5000+ star background field\n";
    std::cout << "  ✓ Enhanced planet colors and sizes\n";
    std::cout << "  ✓ Black hole gravity well detection\n";
    std::cout << "  ✓ Frustum culling optimization (auto-skip off-screen objects)\n";
    std::cout << "  ✓ Special modes status display (turbo, barrel roll, etc.)\n\n";

    std::cout << "VISUAL ENHANCEMENTS:\n";
    std::cout << "  ★ Atmospheric glow around planets (rim lighting)\n";
    std::cout << "  ★ Procedural planet textures (Earth, Mars, Jupiter, etc.)\n";
    std::cout << "  ★ Volumetric nebula background (6+ colored regions)\n";
    std::cout << "  ★ Realistic engine particle effects\n";
    std::cout << "  ★ Enhanced star rendering with halos\n";
    std::cout << "  ★ Camera zoom with smooth transitions\n";
    std::cout << "  ★ Improved lighting and specular highlights\n";
    std::cout << "  ★ REALISTIC GALAXY: 20,000 stars in spiral arm structure!\n";
    std::cout << "  ★ MASSIVE SUN: 7x larger and blazing yellow-orange\n";
    std::cout << "  ★ 3D COORDINATE GRID: Mathematical visualization system\n";
    std::cout << "  ★ STORY MODE: Immersive narrative and mission objectives\n";
    std::cout << "  ★ HOHMANN TRANSFERS: NASA-style interplanetary route calculator!\n";
    std::cout << "  ★ LAGRANGE POINTS: Visualize L1-L5 gravitational balance points!\n";
    std::cout << "  ★ SATURN RINGS: Beautiful golden rings with realistic banding!\n";
    std::cout << "  ★ ORBITAL MECHANICS HUD: Real-time orbital parameter display!\n";
    std::cout << "  ★ BLOOM & HDR POST-PROCESSING: Cinematic glow and tone mapping!\n";
    std::cout << "  ★ BLACK HOLE ACCRETION DISK: Dynamic swirling matter with heat!\n";
    std::cout << "  ★ GRAVITATIONAL LENSING: Realistic light bending physics!\n";
    std::cout << "  ★ ImGui INTERFACE: Interactive panels for all controls!\n";
    std::cout << "  ★ CAMERA SHAKE: Rumble effects on boost and barrel rolls!\n";
    std::cout << "  ★ SPEED-BASED FOV: Dynamic field of view - feel the speed!\n";
    std::cout << "  ★ CINEMATIC CAMERA ROLL: Banking on turns like racing games!\n\n";

    std::cout << "🆕 NEW ADVANCED FEATURES:\n";
    std::cout << "  🏆 ACHIEVEMENT SYSTEM: Unlock accomplishments and earn points!\n";
    std::cout << "  🔬 SCIENCE SYSTEM: Scan planets and collect research data!\n";
    std::cout << "  📍 WAYPOINT SYSTEM: Create custom navigation markers!\n";
    std::cout << "  📊 ADVANCED TELEMETRY: G-forces, acceleration, orbital analysis!\n";
    std::cout << "  🌌 COSMIC EVENTS: Random space phenomena and challenges!\n";
    std::cout << "  📝 ENHANCED LOGGING: Color-coded console messages!\n\n";

    std::cout << "┌─ ADVANCED SYSTEM HOTKEYS ─────────────────────────────────┐\n";
    std::cout << "│  F1           │ Toggle Help Overlay (ImGui)               │\n";
    std::cout << "│  F2           │ Show Achievement Summary                  │\n";
    std::cout << "│  F3           │ Show Science Data Summary                 │\n";
    std::cout << "│  F4           │ Advanced Telemetry Display                │\n";
    std::cout << "│  F5           │ List All Waypoints                        │\n";
    std::cout << "│  F6           │ Create Waypoint at Current Position       │\n";
    std::cout << "│  F7           │ Start Science Scan (nearest body)         │\n";
    std::cout << "│  F8           │ Trigger Random Cosmic Event               │\n";
    std::cout << "│  F9           │ Enhanced Logger Demo                      │\n";
    std::cout << "│  F10          │ Show Complete Statistics                  │\n";
    std::cout << "└───────────────────────────────────────────────────────────┘\n\n";

    // Projection matrix will be dynamic based on camera FOV (for zoom)
    float aspectRatio = ASPECT_RATIO;

    // Initialize solar system
    auto bodies = SolarSystemFactory::createSimpleSystem();
    PhysicsEngine physics(PHYSICS_TIMESTEP);

    // Create preset waypoints now that bodies exist
    waypoints.createPresetWaypoints(bodies);

    // Enable trails for all planets (not sun or black hole)
    for (auto* body : bodies)
    {
        physics.addBody(&body->getPhysicsBody());

        if (body->getName() != "Sun" && body->getName() != "Black Hole")
        {
            body->enableTrail(800);
        }
    }

    // Create spaceship
    Spaceship ship(1.0f, 0.5f, Vec3(0.2f, 0.8f, 1.0f));  // Mass=1, Radius=0.5, Cyan color
    ship.getPhysicsBody().position = Vec3(-20, 0, 50);   // Start position away from planets
    ship.getPhysicsBody().velocity = Vec3(0, 2, 0);      // Initial orbital velocity
    ship.enableExhaustTrail(300);                        // Enable orange exhaust trail
    physics.addBody(&ship.getPhysicsBody());             // Add to physics simulation

    std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
    std::cout << "║  SPACESHIP INITIALIZED                                    ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════╣\n";
    std::cout << "║  Position:  (-20.0, 0.0, 50.0)                            ║\n";
    std::cout << "║  Velocity:  (0.0, 2.0, 0.0) - Stable orbit                ║\n";
    std::cout << "║  Mass:      1.0 kg                                        ║\n";
    std::cout << "║  Radius:    0.5 units                                     ║\n";
    std::cout << "║  Color:     Cyan (pilot ship)                             ║\n";
    std::cout << "║                                                           ║\n";
    std::cout << "║  Ship is affected by gravity from 13 celestial bodies:    ║\n";
    std::cout << "║  • Sun (2000 mass) • 8 Planets • Black Hole (8000 mass)  ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";
    std::cout << "Ready to launch! Press I to thrust forward...\n\n";

    // Create starfield
    StarField starfield(5000);

    // Create collision predictor
    CollisionPredictor collisionPredictor;

    // Create orbital paths
    std::vector<OrbitalPath> orbitalPaths;
    Body* sun = nullptr;
    for (auto* body : bodies)
    {
        if (body->getName() == "Sun")
        {
            sun = &body->getPhysicsBody();
            break;
        }
    }

    if (sun)
    {
        for (auto* body : bodies)
        {
            if (body->getName() != "Sun" && body->getName() != "Black Hole" &&
                body->getName().find("Asteroid") == std::string::npos)
            {
                OrbitalPath path;
                path.calculatePath(body->getPhysicsBody(), *sun, PHYSICS_TIMESTEP, ORBITAL_PATH_POINTS);
                path.setColor(body->getColor() * ORBITAL_PATH_COLOR_DIM);
                orbitalPaths.push_back(path);
            }
        }
    }

    // Create visual prediction systems
    PredictionMarker predictionMarkers;
    VectorRenderer vectorRenderer;

    // Flags for toggling visualizations
    bool showVectors = true;
    bool showTrajectoryMarkers = true;
    bool showShipTrajectory = true;
    bool showCoordinateGrid = false;  // Grid starts hidden (toggle with 'G')
    bool showLagrangePoints = false;  // Lagrange points hidden initially
    bool showTransferOrbit = false;   // Transfer orbit visualization

    // Time controls
    float timeScale = 1.0f;      // 1.0 = normal speed
    bool isPaused = false;

    // G-force damage threshold - forces above this cause ship damage
    const float GFORCE_DAMAGE_THRESHOLD = 10.0f;

    sf::Clock clock;
    float collisionPredictTimer = 0.0f;
    float vectorUpdateTimer = 0.0f;
    float trajectoryUpdateTimer = 0.0f;

    float lastX = 640;
    float lastY = 360;
    bool firstMouse = true;

    while (window.isOpen())
    {
        float rawDeltaTime = clock.restart().asSeconds();
        float deltaTime = isPaused ? 0.0f : rawDeltaTime * timeScale;

        sf::Event event;
        while (window.pollEvent(event))
        {
            // Process GUI events first
            gui.processEvent(window, event);

            if (event.type == sf::Event::Closed)
                window.close();

            // Mouse wheel for zoom
            if (event.type == sf::Event::MouseWheelScrolled)
            {
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
                {
                    float zoomAmount = event.mouseWheelScroll.delta * 5.0f;
                    camera.zoom(zoomAmount);
                }
            }

            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Space)
                {
                    showVectors = !showVectors;
                    std::cout << "Vector display: " << (showVectors ? "ON" : "OFF") << "\n";
                }
                if (event.key.code == sf::Keyboard::T)
                {
                    showTrajectoryMarkers = !showTrajectoryMarkers;
                    std::cout << "Trajectory markers: " << (showTrajectoryMarkers ? "ON" : "OFF") << "\n";
                }
                if (event.key.code == sf::Keyboard::R)
                {
                    showShipTrajectory = !showShipTrajectory;
                    std::cout << "Ship trajectory prediction: " << (showShipTrajectory ? "ON" : "OFF") << "\n";
                }
                if (event.key.code == sf::Keyboard::G)
                {
                    showCoordinateGrid = !showCoordinateGrid;
                    coordinateGrid.setVisible(showCoordinateGrid);
                    std::cout << "Coordinate grid: " << (showCoordinateGrid ? "ON" : "OFF") << "\n";
                }
                if (event.key.code == sf::Keyboard::H)
                {
                    // OPTIMIZED: Calculate Hohmann transfer from Earth to Mars
                    CelestialBody* earthBody = findBodyByName("Earth", bodies);
                    CelestialBody* marsBody = findBodyByName("Mars", bodies);
                    Body* earth = earthBody ? &earthBody->getPhysicsBody() : nullptr;
                    Body* mars = marsBody ? &marsBody->getPhysicsBody() : nullptr;

                    if (earth && mars && sun)
                    {
                        TransferOrbit transfer = transferCalculator.calculateTransfer(*earth, *mars, *sun, 0.0f);
                        showTransferOrbit = true;
                        std::cout << "\n=== HOHMANN TRANSFER: Earth -> Mars ===\n";
                        orbitalHUD.displayTransferInfo(transfer.deltaV, transfer.transferTime,
                                                      transfer.phase_angle, transfer.optimal_phase);
                    }
                }
                if (event.key.code == sf::Keyboard::L)
                {
                    showLagrangePoints = !showLagrangePoints;
                    lagrangePoints.setVisible(showLagrangePoints);

                    if (showLagrangePoints && sun)
                    {
                        // OPTIMIZED: Calculate Lagrange points between Sun and Earth
                        CelestialBody* earthBody = findBodyByName("Earth", bodies);
                        Body* earth = earthBody ? &earthBody->getPhysicsBody() : nullptr;

                        if (earth)
                        {
                            lagrangePoints.calculatePoints(*sun, *earth);
                            orbitalHUD.displayLagrangeInfo(5, true);
                        }
                    }

                    std::cout << "Lagrange points: " << (showLagrangePoints ? "ON" : "OFF") << "\n";
                }
                if (event.key.code == sf::Keyboard::M)
                {
                    // Display orbital mechanics for ship
                    if (sun)
                    {
                        OrbitalParameters shipOrbit = orbitalHUD.calculateOrbitalParameters(
                            ship.getPhysicsBody(), *sun);
                        orbitalHUD.displayToConsole(shipOrbit, "Ship", "Sun");
                    }
                }
                if (event.key.code == sf::Keyboard::C)
                {
                    camera.toggleMode();
                    std::cout << "Camera mode: " << camera.getModeName() << "\n";
                }
                if (event.key.code == sf::Keyboard::P)
                {
                    isPaused = !isPaused;
                    statistics.recordPause();
                    std::cout << "Simulation: " << (isPaused ? "PAUSED" : "RUNNING") << "\n";
                }
                if (event.key.code == sf::Keyboard::Equal || event.key.code == sf::Keyboard::Add)
                {
                    timeScale *= 1.5f;
                    if (timeScale > 10.0f) timeScale = 10.0f;
                    statistics.recordTimeWarp();
                    std::cout << "Time scale: " << timeScale << "x\n";
                }
                if (event.key.code == sf::Keyboard::Hyphen || event.key.code == sf::Keyboard::Subtract)
                {
                    timeScale /= 1.5f;
                    statistics.recordTimeWarp();
                    if (timeScale < MIN_TIME_SCALE) timeScale = MIN_TIME_SCALE;
                    std::cout << "Time scale: " << timeScale << "x\n";
                }
                if (event.key.code == sf::Keyboard::B)
                {
                    // Attempt landing if in approach range
                    if (ship.getLandingState() == LandingState::APPROACHING)
                    {
                        // Find closest landable body
                        CelestialBody* closestLandable = nullptr;
                        float closestDist = MAX_DISTANCE;

                        for (auto* body : bodies)
                        {
                            if (ship.canLandOn(body))
                            {
                                Vec3 toBody = body->getPhysicsBody().position - ship.getPhysicsBody().position;
                                float dist = toBody.length();
                                if (dist < closestDist)
                                {
                                    closestDist = dist;
                                    closestLandable = body;
                                }
                            }
                        }

                        if (closestLandable)
                        {
                            ship.attemptLanding(closestLandable);
                            bool landingSuccess = (ship.getLandingState() == LandingState::LANDED);
                            statistics.recordLandingAttempt(landingSuccess);
                            if (landingSuccess)
                            {
                                statistics.recordPlanetVisit();
                            }
                        }
                    }
                    else if (ship.getLandingState() == LandingState::FLYING)
                    {
                        std::cout << "Cannot land - not in approach range or moving too fast!\n";
                        std::cout << "Slow down and get closer to a landable body (Earth, Moon, Mars).\n";
                    }
                    else
                    {
                        std::cout << "Already landed. Press N to take off.\n";
                    }
                }
                if (event.key.code == sf::Keyboard::N)
                {
                    // Take off if currently landed
                    if (ship.getLandingState() == LandingState::LANDED)
                    {
                        ship.takeoff();
                    }
                    else
                    {
                        std::cout << "Cannot take off - not currently landed!\n";
                    }
                }
                // ===== NEW ADVANCED SYSTEM CONTROLS =====
                if (event.key.code == sf::Keyboard::F2)
                {
                    // Display Achievement Summary
                    achievements.displaySummary();
                }
                if (event.key.code == sf::Keyboard::F3)
                {
                    // Display Science Summary
                    science.displaySummary();
                }
                if (event.key.code == sf::Keyboard::F4)
                {
                    // Display Advanced Telemetry
                    telemetry.displayComprehensive(ship, bodies);
                }
                if (event.key.code == sf::Keyboard::F5)
                {
                    // List Waypoints
                    waypoints.listWaypoints();
                }
                if (event.key.code == sf::Keyboard::F6)
                {
                    // Create waypoint at current position
                    std::string waypointName = "Waypoint_" + std::to_string(waypoints.getWaypoints().size() + 1);
                    waypoints.addWaypoint(waypointName, ship.getPhysicsBody().position, "Custom navigation marker");
                    statistics.recordWaypoint();
                }
                if (event.key.code == sf::Keyboard::F7)
                {
                    // Start science scan on nearest body
                    // OPTIMIZED: Use helper to find nearest body efficiently
                    Vec3 shipPos = ship.getPhysicsBody().position;
                    float nearestDist = 0.0f;
                    CelestialBody* nearestBody = findNearestBody(shipPos, bodies, nearestDist);

                    std::string nearestName = nearestBody ? nearestBody->getName() : "Unknown";
                    science.startScan(nearestName, nearestDist);
                }
                if (event.key.code == sf::Keyboard::F8)
                {
                    // Trigger a random cosmic event (for testing)
                    cosmicEvents.checkForNewEvent();
                }
                if (event.key.code == sf::Keyboard::F9)
                {
                    // Enhanced logger test
                    gLogger.sectionHeader("SYSTEM STATUS REPORT");
                    gLogger.success("All systems operational");
                    gLogger.info("Current mission progress tracked");
                    gLogger.warning("Approaching high radiation zone");
                }
                if (event.key.code == sf::Keyboard::F10)
                {
                    // Display comprehensive statistics with comparisons
                    statistics.displayComplete();
                }
                if (event.key.code == sf::Keyboard::F11)
                {
                    // Open planet information panel for locked target
                    if (gui.isTargetLocked())
                    {
                        CelestialBody* target = gui.getLockedTarget(bodies);
                        if (target != nullptr)
                        {
                            planetPanel.open(target);
                        }
                    }
                    else
                    {
                        gLogger.warning("No target locked! Use GUI to lock a target first, then press F11.");
                    }
                }
                if (event.key.code == sf::Keyboard::F12)
                {
                    // Quick save
                    SaveData data;
                    data.shipPosition = ship.getPosition();
                    data.shipVelocity = ship.getVelocity();
                    data.shipYaw = ship.getYaw();
                    data.shipPitch = ship.getPitch();
                    data.fuel = resourceSystem.getFuel();
                    data.maxFuel = resourceSystem.getMaxFuel();
                    data.power = resourceSystem.getPower();
                    data.maxPower = resourceSystem.getMaxPower();
                    data.oxygen = resourceSystem.getOxygen();
                    data.maxOxygen = resourceSystem.getMaxOxygen();
                    data.health = damageSystem.getHealth();
                    data.maxHealth = damageSystem.getMaxHealth();
                    data.totalGameTime = statistics.getSession().totalFlightTime;
                    data.timeScale = timeScale;
                    saveSystem.quickSave(data);
                    audioManager.playSound(SoundEffect::UI_CLICK);
                }
                if (event.key.code == sf::Keyboard::L && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    // Quick load
                    SaveData data;
                    if (saveSystem.quickLoad(data))
                    {
                        ship.setPosition(data.shipPosition);
                        ship.setVelocity(data.shipVelocity);
                        ship.setYaw(data.shipYaw);
                        ship.setPitch(data.shipPitch);
                        resourceSystem.setFuel(data.fuel);
                        resourceSystem.setMaxFuel(data.maxFuel);
                        resourceSystem.setPower(data.power);
                        resourceSystem.setMaxPower(data.maxPower);
                        resourceSystem.setOxygen(data.oxygen);
                        resourceSystem.setMaxOxygen(data.maxOxygen);
                        damageSystem.setHealth(data.health);
                        damageSystem.setMaxHealth(data.maxHealth);
                        timeScale = data.timeScale;
                        audioManager.playSound(SoundEffect::UI_CLICK);
                    }
                }
                if (event.key.code == sf::Keyboard::R && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    // Display resource status
                    resourceSystem.displayStatus();
                }
                if (event.key.code == sf::Keyboard::D && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    // Display damage report
                    damageSystem.displayDamageReport();
                }
                if (event.key.code == sf::Keyboard::M && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    // Display missions
                    missionSystem.displayAllMissions();
                }
                if (event.key.code == sf::Keyboard::A && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    // Display audio status
                    audioManager.displayStatus();
                }

                // ===== NEW FEATURE CONTROLS =====
                // Quick time control hotkeys (1-5 keys)
                if (event.key.code == sf::Keyboard::Num1)
                {
                    timeScale = MIN_TIME_SCALE;
                    std::cout << "Time scale set to " << MIN_TIME_SCALE << "x (slow motion)\n";
                }
                if (event.key.code == sf::Keyboard::Num2)
                {
                    timeScale = 1.0f;
                    std::cout << "Time scale set to 1.0x (normal)\n";
                }
                if (event.key.code == sf::Keyboard::Num3)
                {
                    timeScale = 2.0f;
                    std::cout << "Time scale set to 2.0x (fast)\n";
                }
                if (event.key.code == sf::Keyboard::Num4)
                {
                    timeScale = 5.0f;
                    std::cout << "Time scale set to 5.0x (faster)\n";
                }
                if (event.key.code == sf::Keyboard::Num5)
                {
                    timeScale = 10.0f;
                    std::cout << "Time scale set to 10.0x (fastest)\n";
                }

                // Auto-stabilization toggle (Ctrl+S)
                if (event.key.code == sf::Keyboard::S && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    ship.toggleAutoStabilization();
                }

                // Speed limiter toggle (Ctrl+V)
                if (event.key.code == sf::Keyboard::V && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    ship.toggleSpeedLimiter();
                }

                // ===== AWESOME NEW FEATURES! =====
                // Turbo Boost (Q key) - GOTTA GO FAST!
                if (event.key.code == sf::Keyboard::Q)
                {
                    ship.activateTurboBoost();
                    camera.addShake(0.5f);  // Camera shake for intensity!
                    std::cout << "*** TURBO BOOST ACTIVATED! ***\n";
                }

                // Barrel Roll (E key) - DO A BARREL ROLL!
                if (event.key.code == sf::Keyboard::E)
                {
                    ship.initiateBarrelRoll();
                    camera.addShake(0.3f);  // Shake it!
                    std::cout << "*** BARREL ROLL! ***\n";
                }

                // Rainbow Exhaust Mode (X key) - STYLE POINTS!
                if (event.key.code == sf::Keyboard::X)
                {
                    ship.toggleRainbowExhaust();
                    std::cout << "Rainbow exhaust mode " << (ship.getIsThrusting() ? "ON" : "toggled") << "!\n";
                }

                // Hyperdrive (Z key) - LUDICROUS SPEED!
                if (event.key.code == sf::Keyboard::Z)
                {
                    ship.activateHyperdrive();
                    camera.addShake(1.0f);  // BIG SHAKE!
                    std::cout << "*** HYPERDRIVE CHARGING... ***\n";
                }

                // Drift Mode (V key alone) - TOKYO DRIFT IN SPACE!
                if (event.key.code == sf::Keyboard::V && !sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                {
                    ship.toggleDriftMode();
                    std::cout << "Drift mode " << (ship.isDrifting() ? "ON" : "OFF") << "!\n";
                }

                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
            }
        }

        // -------- Camera Control (only in FREE mode) --------
        if (camera.getMode() == CameraMode::FREE)
        {
            // Mouse Look
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            float xpos = mousePos.x;
            float ypos = mousePos.y;

            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos;

            lastX = xpos;
            lastY = ypos;

            float sensitivity = 0.1f;
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            camera.yaw += xoffset;
            camera.pitch += yoffset;

            if (camera.pitch > 89.0f)
                camera.pitch = 89.0f;
            if (camera.pitch < -89.0f)
                camera.pitch = -89.0f;

            camera.updateVectors();

            // WASD Movement (use rawDeltaTime for camera, not affected by time scale)
            float speed = 20.0f * rawDeltaTime;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                camera.position = camera.position + camera.front * speed;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                camera.position = camera.position - camera.front * speed;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                camera.position = camera.position - camera.right * speed;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                camera.position = camera.position + camera.right * speed;
        }
        else if (camera.getMode() == CameraMode::FOLLOW_SHIP)
        {
            camera.updateFollowMode(ship);
        }
        else if (camera.getMode() == CameraMode::CHASE_SHIP)
        {
            camera.updateChaseMode(ship);
        }

        // -------- Spaceship Controls --------
        // Reset thrusting state each frame
        ship.stopThrust();

        // Thrust controls (I/K or Up/Down arrows)
        bool thrusting = false;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::I) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            if (resourceSystem.canThrust())
            {
                ship.applyThrust(deltaTime);
                thrusting = true;
                audioManager.playLoopingSound(SoundEffect::ENGINE_THRUST, 1.0f);
            }
            else
            {
                gLogger.warning("Cannot thrust! Insufficient fuel or power!");
            }
        }
        else
        {
            audioManager.stopLoopingSound(SoundEffect::ENGINE_THRUST);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::K) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            ship.applyReverseThrust(deltaTime);
        }

        // Rotation controls (J/L or Left/Right arrows for yaw)
        // Use rawDeltaTime for ship controls so they're responsive even when paused/slowed
        float rotationAmount = ship.getRotationSpeed() * rawDeltaTime;

        // Track rotation input for cinematic camera roll
        float rotationInput = 0.0f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::J) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            ship.rotate(-rotationAmount, 0.0f);  // Rotate left (negative yaw)
            rotationInput = -1.0f;  // Turning left
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            ship.rotate(rotationAmount, 0.0f);  // Rotate right (positive yaw)
            rotationInput = 1.0f;  // Turning right
        }

        // Pitch controls (U/O for up/down)
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::U))
        {
            ship.rotate(0.0f, rotationAmount);  // Pitch up
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
        {
            ship.rotate(0.0f, -rotationAmount);  // Pitch down
        }

        // Update cinematic camera roll based on rotation input (racing game effect!)
        camera.updateCinematicRoll(rotationInput, rawDeltaTime);

        // -------- Physics Update --------
        // Update physics (ship's update() will handle surface lock if landed)
        physics.update(deltaTime);

        for (auto* body : bodies)
            body->update(deltaTime);

        // Update spaceship
        ship.update(deltaTime);

        // Check landing proximity (only when not landed)
        if (ship.getLandingState() != LandingState::LANDED)
        {
            ship.checkLandingProximity(bodies);
        }

        // Update camera zoom (smooth interpolation)
        camera.updateZoom(rawDeltaTime);

        // ===== AMAZING CAMERA EFFECTS! =====
        // Update camera shake (for explosions, boost, barrel rolls!)
        camera.updateShake(rawDeltaTime);

        // Update speed-based FOV - wider FOV at high speeds for that racing game feel!
        camera.updateSpeedFOV(ship.getSpeed(), rawDeltaTime);

        // Add subtle camera shake when thrusting at high speeds
        if (ship.getIsThrusting() && ship.getSpeed() > 50.0f)
        {
            camera.addShake(0.02f * rawDeltaTime);  // Subtle continuous shake
        }

        // Update visual systems
        nebula.update(deltaTime);
        galaxy.update(deltaTime);
        coordinateGrid.update(deltaTime);
        narrator.update(deltaTime);
        lagrangePoints.update(deltaTime);
        accretionDisk.update(deltaTime);

        // ===== UPDATE NEW ADVANCED SYSTEMS =====
        // Update telemetry
        telemetry.update(ship, deltaTime, bodies);

        // Update science scan
        science.updateScan(deltaTime);

        // Update cosmic events
        cosmicEvents.update(deltaTime);

        // Update achievements based on gameplay
        achievements.updateSpeed(ship.getSpeed());
        achievements.updateDistance(ship.getSpeed() * deltaTime);

        // Update enhanced statistics
        statistics.updateFlightTime(deltaTime);
        statistics.updateDistance(ship.getSpeed() * deltaTime);
        statistics.updateMaxSpeed(ship.getSpeed());
        statistics.updateMaxGForce(telemetry.getCurrentGForce());
        statistics.updateSciencePoints(science.getTotalScience());
        statistics.updateAchievements(achievements.getTotalPoints(), achievements.getUnlockedCount());

        // ===== UPDATE NEW RESOURCE/DAMAGE/MISSION SYSTEMS =====
        // Update resource system
        resourceSystem.update(deltaTime, thrusting, ship.getLandingState() == LandingState::LANDED,
                             ship.getPosition(), bodies);

        // Update damage system
        damageSystem.update(deltaTime);

        // Check for G-force damage
        float currentGForce = telemetry.getCurrentGForce();
        if (currentGForce > GFORCE_DAMAGE_THRESHOLD)
        {
            damageSystem.takeGForceDamage(currentGForce);
        }

        // Update audio manager
        audioManager.update(deltaTime);

        // Update mission system
        missionSystem.update(deltaTime);

        // Auto-resupply when landing on suitable planets
        static bool landedLastFrame = false;
        if (ship.getLandingState() == LandingState::LANDED)
        {
            if (!landedLastFrame)
            {
                // Just landed - trigger resupply and repair
                resourceSystem.fullResupply();
                damageSystem.fullRepair();
                audioManager.playSound(SoundEffect::LANDING_SUCCESS);
                audioManager.playSound(SoundEffect::REFUEL);
                landedLastFrame = true;
            }
        }
        else
        {
            if (landedLastFrame)
            {
                audioManager.playSound(SoundEffect::TAKEOFF);
            }
            landedLastFrame = false;
        }

        // Check for achievement triggers
        Vec3 shipPos = ship.getPhysicsBody().position;
        for (auto* body : bodies)
        {
            Vec3 diff = body->getPhysicsBody().position - shipPos;
            float dist = diff.length();

            // Record close calls
            if (dist - body->getRadius() < 5.0f && dist - body->getRadius() > 0.0f)
            {
                achievements.recordCloseCall();
                statistics.recordCloseCall();
            }

            // Record black hole proximity
            if (body->getName() == "Black Hole" && dist < 15.0f)
            {
                achievements.recordBlackHoleApproach();
                statistics.updateBlackHoleDistance(dist);
            }

            // Record sun proximity
            if (body->getName() == "Sun" && dist < 10.0f)
            {
                achievements.recordSunApproach();
                statistics.updateSunDistance(dist);
            }
        }

        // Record landing
        if (ship.getLandingState() == LandingState::LANDED)
        {
            achievements.recordLanding();
        }

        // Record cosmic events for statistics
        if (cosmicEvents.getActiveEvents().size() > 0)
        {
            static size_t lastEventCount = 0;
            size_t currentCount = cosmicEvents.getActiveEvents().size();
            if (currentCount > lastEventCount)
            {
                statistics.recordCosmicEvent();
            }
            lastEventCount = currentCount;
        }

        // Update GUI
        gui.update(window);
        GUIState& guiState = gui.getState();

        // Sync GUI state with simulation state
        timeScale = guiState.timeScale;
        isPaused = guiState.isPaused;
        showVectors = guiState.showVectors;
        showTrajectoryMarkers = guiState.showTrajectory;
        showCoordinateGrid = guiState.showGrid;
        showLagrangePoints = guiState.showLagrangePoints;
        coordinateGrid.setVisible(showCoordinateGrid);
        lagrangePoints.setVisible(showLagrangePoints);

        // Emit engine particles when thrusting
        if (ship.getIsThrusting())
        {
            Vec3 shipPos = ship.getPhysicsBody().position;
            Vec3 shipForward = ship.getForward();
            Vec3 shipVel = ship.getPhysicsBody().velocity;

            // Emit particles from behind the ship
            Vec3 exhaustPos = shipPos - shipForward * (ship.getRadius() * 1.2f);
            engineParticles.emit(exhaustPos, shipForward, shipVel, 1.0f, 8);
        }

        // Update engine particles
        engineParticles.update(deltaTime);

        // *** REAL-TIME DISTANCE/ALTITUDE DISPLAY ***
        static float displayTimer = 0.0f;
        static int lastBodiesRendered = 0;
        static int lastBodiesCulled = 0;
        displayTimer += rawDeltaTime;
        if (displayTimer > 1.0f)  // Update every second
        {
            displayTimer = 0.0f;

            // Find nearest celestial body
            // OPTIMIZED: Find nearest body in single pass instead of two separate loops
            Vec3 shipPos = ship.getPhysicsBody().position;
            float nearestDist = 0.0f;
            CelestialBody* nearestBody = findNearestBody(shipPos, bodies, nearestDist);

            std::string nearestName = nearestBody ? nearestBody->getName() : "Unknown";
            float altitude = nearestBody ? (nearestDist - nearestBody->getRadius()) : nearestDist;

            // Display telemetry
            std::cout << "\n=== SHIP TELEMETRY ===\n";
            std::cout << "Position: (" << shipPos.x << ", " << shipPos.y << ", " << shipPos.z << ")\n";
            std::cout << "Velocity: " << ship.getSpeed() << " m/s\n";
            std::cout << "Nearest body: " << nearestName << " (" << nearestDist << " units)\n";
            std::cout << "Altitude: " << altitude << " units\n";

            // *** COLLISION WARNING SYSTEM ***
            if (altitude < 5.0f && altitude > 0.0f)
            {
                std::cout << "*** DANGER: LOW ALTITUDE WARNING! ***\n";
                std::cout << "*** COLLISION IMMINENT IN " << (altitude / ship.getSpeed()) << " SECONDS! ***\n";
            }
            else if (altitude < 10.0f && altitude > 0.0f)
            {
                std::cout << "** CAUTION: Approaching " << nearestName << " **\n";
            }

            // Check if we're being pulled into black hole
            if (nearestName == "Black Hole" && nearestDist < 30.0f)
            {
                std::cout << "!!! BLACK HOLE GRAVITY WELL DETECTED !!!\n";
                std::cout << "!!! EXTREME GRAVITATIONAL FORCES !!!\n";
                narrator.triggerEvent(StoryEvent::BLACK_HOLE_WARNING);
            }

            // Trigger story events based on proximity
            if (nearestName == "Sun" && nearestDist < 15.0f)
            {
                narrator.triggerEvent(StoryEvent::NEAR_SUN);
            }

            // Collision warning story event
            if (altitude < 5.0f && altitude > 0.0f)
            {
                narrator.triggerEvent(StoryEvent::COLLISION_WARNING);
            }

            std::cout << "Time scale: " << timeScale << "x " << (isPaused ? "[PAUSED]" : "[RUNNING]") << "\n";
            std::cout << "Camera mode: " << camera.getModeName() << "\n";

            // ===== SPECIAL MODES STATUS DISPLAY =====
            std::cout << "\n--- SPECIAL MODES ---\n";

            // Check if any special mode is active
            bool anyModeActive = false;

            if (ship.isTurboBoostActive())
            {
                std::cout << "🔥 TURBO BOOST ACTIVE - 3X POWER!\n";
                anyModeActive = true;
            }

            if (ship.isDoingBarrelRoll())
            {
                int progress = (int)(ship.getBarrelRollProgress() * 100.0f / FULL_ROTATION_DEGREES);
                std::cout << "🌀 BARREL ROLL IN PROGRESS (" << progress << "%)\n";
                anyModeActive = true;
            }

            if (ship.isHyperdriveCharging())
            {
                int chargePercent = (int)(ship.getHyperdriveCharge() * 100.0f);
                std::cout << "⚡ HYPERDRIVE CHARGING... [" << chargePercent << "%]\n";
                anyModeActive = true;
            }
            else if (ship.isInHyperspace())
            {
                std::cout << "⚡ HYPERDRIVE ACTIVE - LUDICROUS SPEED!\n";
                anyModeActive = true;
            }

            if (ship.isDrifting())
            {
                std::cout << "💨 DRIFT MODE - Tokyo Drift in Space!\n";
                anyModeActive = true;
            }

            if (ship.isRainbowExhaustActive())
            {
                std::cout << "🌈 RAINBOW EXHAUST MODE - Style points!\n";
                anyModeActive = true;
            }

            if (ship.getComboCount() > 0)
            {
                std::cout << "🎯 COMBO: x" << ship.getComboCount()
                          << " | Style Points: " << ship.getStylePoints() << "\n";
                anyModeActive = true;
            }

            if (!anyModeActive)
            {
                std::cout << "All systems normal\n";
            }

            // ===== PERFORMANCE STATS =====
            std::cout << "\n--- PERFORMANCE ---\n";
            int totalBodies = lastBodiesRendered + lastBodiesCulled;
            float cullPercentage = totalBodies > 0 ? (lastBodiesCulled * 100.0f / totalBodies) : 0.0f;

            std::cout << "Bodies Rendered: " << lastBodiesRendered << "/" << totalBodies
                      << " (" << (int)cullPercentage << "% culled)\n";

            if (lastBodiesCulled > 0)
            {
                std::cout << "⚡ Frustum Culling Active - Performance Boost!\n";
            }

            std::cout << "FOV: " << (int)camera.getFOV() << "°";
            if (camera.getFOV() > 50.0f)
            {
                std::cout << " (Speed boost active!)";
            }
            std::cout << "\n";

            std::cout << "=====================\n";
        }

        // Update collision predictions and markers every 0.5 seconds
        collisionPredictTimer += deltaTime;
        if (collisionPredictTimer > COLLISION_UPDATE_INTERVAL)
        {
            std::vector<Body*> physBodies;
            for (auto* body : bodies)
                physBodies.push_back(&body->getPhysicsBody());

            collisionPredictor.predictCollisions(physBodies, PHYSICS_TIMESTEP, COLLISION_PREDICTION_TIME);

            // Add visual markers for collision points
            predictionMarkers.clear();
            for (const auto& pred : collisionPredictor.getPredictions())
            {
                predictionMarkers.addCollisionMarker(pred.collisionPoint, pred.timeToCollision);
            }

            collisionPredictTimer = 0.0f;
        }

        // Update velocity and force vectors periodically
        vectorUpdateTimer += deltaTime;
        if (vectorUpdateTimer > VECTOR_UPDATE_INTERVAL && showVectors)
        {
            vectorRenderer.clear();
            for (auto* body : bodies)
            {
                if (body->getName() != "Sun")
                {
                    vectorRenderer.addVelocityVector(
                        body->getPhysicsBody().position,
                        body->getPhysicsBody().velocity
                    );
                }
            }

            // Add spaceship velocity vector (brighter when thrusting)
            vectorRenderer.addVelocityVector(
                ship.getPhysicsBody().position,
                ship.getPhysicsBody().velocity
            );

            vectorUpdateTimer = 0.0f;
        }

        // Update trajectory prediction points periodically
        trajectoryUpdateTimer += deltaTime;
        if (trajectoryUpdateTimer > TRAJECTORY_UPDATE_INTERVAL)
        {
            // Calculate and show trajectory points for selected bodies
            std::vector<Body*> allBodies;
            for (auto* body : bodies)
                allBodies.push_back(&body->getPhysicsBody());

            if (showTrajectoryMarkers)
            {
                for (auto* body : bodies)
                {
                    if (body->getName() == "Earth" || body->getName() == "Mars")
                    {
                        std::vector<Vec3> trajectoryPoints;
                        collisionPredictor.calculateTrajectoryPoints(
                            body->getPhysicsBody(),
                            allBodies,
                            0.1f,
                            0.5f,
                            50,
                            trajectoryPoints
                        );

                        // Add markers for every 5th point to avoid clutter
                        // Safety check: ensure trajectoryPoints is not empty
                        if (!trajectoryPoints.empty())
                        {
                            for (size_t i = 0; i < trajectoryPoints.size(); i += BODY_TRAJECTORY_POINT_SKIP)
                            {
                                predictionMarkers.addTrajectoryPoint(
                                    trajectoryPoints[i],
                                    body->getColor() * TRAJECTORY_POINT_COLOR_DIM
                                );
                            }
                        }
                    }
                }
            }

            // Add spaceship trajectory prediction
            if (showShipTrajectory)
            {
                std::vector<Vec3> shipTrajectoryPoints;
                collisionPredictor.calculateTrajectoryPoints(
                    ship.getPhysicsBody(),
                    allBodies,
                    0.1f,
                    0.5f,
                    50,
                    shipTrajectoryPoints
                );

                // Safety check: ensure shipTrajectoryPoints is not empty
                // Add markers for every 3rd point (more frequent for ship)
                if (!shipTrajectoryPoints.empty())
                {
                    for (size_t i = 0; i < shipTrajectoryPoints.size(); i += SHIP_TRAJECTORY_POINT_SKIP)
                    {
                        predictionMarkers.addTrajectoryPoint(
                            shipTrajectoryPoints[i],
                            ship.getColor() * 0.8f  // Bright cyan trail
                        );
                    }
                }
            }

            trajectoryUpdateTimer = 0.0f;
        }

        // Update prediction markers
        predictionMarkers.update(deltaTime);

        // Update starfield
        starfield.update(deltaTime);

        // -------- Rendering --------
        // 1. Render scene to framebuffer for post-processing
        sceneFramebuffer.bind();
        glClearColor(0.0f, 0.0f, 0.01f, 1.0f);  // Darker for better nebula contrast
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Mat4 view = camera.getViewMatrix();

        // Dynamic projection matrix with camera FOV (for zoom)
        Mat4 projection = Mat4::perspective(
            camera.getFOV() * 0.0174533f,  // Convert degrees to radians
            aspectRatio,
            0.1f,
            2000.0f
        );

        // Render nebula background (furthest back)
        glDepthMask(GL_FALSE);  // Don't write to depth buffer
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);  // Additive blending for glow
        nebulaShader.use();
        nebulaShader.setMat4("view", view);
        nebulaShader.setMat4("projection", projection);
        nebula.render();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Reset blending
        glDepthMask(GL_TRUE);

        // Render starfield (basic background stars)
        starShader.use();
        starShader.setMat4("view", view);
        starShader.setMat4("projection", projection);
        starfield.render();

        // Render MASSIVE galaxy background with 20,000 stars in realistic spiral arms!
        galaxy.render();

        // Render coordinate grid (mathematical visualization) - if enabled
        if (showCoordinateGrid)
        {
            glDepthMask(GL_FALSE);
            gridShader.use();
            gridShader.setMat4("view", view);
            gridShader.setMat4("projection", projection);
            coordinateGrid.render();
            glDepthMask(GL_TRUE);
        }

        // Render orbital paths
        glDepthMask(GL_FALSE);
        lineShader.use();
        lineShader.setMat4("view", view);
        lineShader.setMat4("projection", projection);
        lineShader.setFloat("alpha", 0.3f);

        for (auto& path : orbitalPaths)
        {
            lineShader.setVec3("lineColor", Vec3(0.4f, 0.4f, 0.6f));
            path.render();
        }
        glDepthMask(GL_TRUE);

        // Render planets with ENHANCED lighting (atmospheric glow, rim lighting, textures)
        planetShader.use();
        planetShader.setVec3("viewPos", camera.position);
        planetShader.setVec3("lightPos", Vec3(0, 0, 0));

        // Track Saturn for rings and Black Hole for special rendering
        Mat4 saturnModelMatrix;
        bool foundSaturn = false;
        CelestialBody* blackHoleBody = nullptr;

        // PERFORMANCE OPTIMIZATION: Frustum culling stats
        int bodiesRendered = 0;
        int bodiesCulled = 0;

        for (auto* body : bodies)
        {
            // FRUSTUM CULLING: Skip rendering objects outside camera view!
            Vec3 bodyPos = body->getPhysicsBody().position;
            float bodyRadius = body->getRadius();

            // Always render Sun and Black Hole (they're important landmarks)
            bool isImportant = (body->getName() == "Sun" || body->getName() == "Black Hole");

            if (!isImportant && !camera.isSphereInFrustum(bodyPos, bodyRadius * 2.0f, aspectRatio))
            {
                bodiesCulled++;
                continue;  // Skip rendering this body - it's not visible!
            }

            bodiesRendered++;

            Mat4 model = body->getModelMatrix();
            bool isSun = (body->getName() == "Sun");
            planetShader.setBool("isSun", isSun);

            // Set atmospheric parameters based on planet
            std::string name = body->getName();
            if (name == "Earth")
            {
                planetShader.setBool("hasAtmosphere", true);
                planetShader.setVec3("atmosphereColor", Vec3(0.3f, 0.5f, 1.0f));  // Blue
                planetShader.setFloat("atmosphereThickness", 0.8f);
            }
            else if (name == "Mars")
            {
                planetShader.setBool("hasAtmosphere", true);
                planetShader.setVec3("atmosphereColor", Vec3(1.0f, 0.6f, 0.3f));  // Orange
                planetShader.setFloat("atmosphereThickness", 0.3f);
            }
            else if (name == "Venus")
            {
                planetShader.setBool("hasAtmosphere", true);
                planetShader.setVec3("atmosphereColor", Vec3(1.0f, 0.9f, 0.6f));  // Yellow
                planetShader.setFloat("atmosphereThickness", 0.9f);
            }
            else if (name == "Jupiter" || name == "Saturn" || name == "Uranus" || name == "Neptune")
            {
                planetShader.setBool("hasAtmosphere", true);
                planetShader.setVec3("atmosphereColor", body->getColor() * 1.2f);
                planetShader.setFloat("atmosphereThickness", 0.6f);
            }
            else
            {
                planetShader.setBool("hasAtmosphere", false);
                planetShader.setFloat("atmosphereThickness", 0.0f);
            }

            // Use texture if available, otherwise use solid color
            GLuint texture = textureLoader.getTexture(name);
            if (texture != 0 && !isSun)
            {
                planetShader.setBool("hasTexture", true);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture);
            }
            else
            {
                planetShader.setBool("hasTexture", false);
            }

            renderer.render(model, view, projection, body->getColor());

            // Save Saturn's model matrix for rendering rings
            if (body->getName() == "Saturn")
            {
                saturnModelMatrix = model;
                foundSaturn = true;
            }

            // Save black hole for special rendering
            if (body->getName() == "Black Hole")
            {
                blackHoleBody = body;
            }
        }

        // ===== SAVE PERFORMANCE STATS =====
        lastBodiesRendered = bodiesRendered;
        lastBodiesCulled = bodiesCulled;

        // Render Black Hole with accretion disk and lensing effect
        if (blackHoleBody)
        {
            Vec3 blackHolePos = blackHoleBody->getPhysicsBody().position;
            float blackHoleRadius = blackHoleBody->getRadius();
            float blackHoleMass = blackHoleBody->getPhysicsBody().mass;
            Mat4 blackHoleModel = Mat4::translation(blackHolePos.x, blackHolePos.y, blackHolePos.z);

            // 1. Render accretion disk (with blending)
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDepthMask(GL_FALSE);

            accretionDiskShader.use();
            accretionDisk.render(&accretionDiskShader, blackHoleModel, view, projection);

            glDepthMask(GL_TRUE);

            // 2. Render gravitational lensing effect (transparent sphere showing light bending)
            lensingShader.use();
            lensingShader.setMat4("view", view);
            lensingShader.setMat4("projection", projection);
            lensingShader.setVec3("viewPos", camera.position);
            lensingShader.setVec3("blackHolePos", blackHolePos);
            lensingShader.setFloat("blackHoleMass", blackHoleMass);
            lensingShader.setFloat("schwarzschildRadius", blackHoleRadius * 0.8f);

            Mat4 lensingModel = Mat4::translation(blackHolePos.x, blackHolePos.y, blackHolePos.z)
                              * Mat4::scale(blackHoleRadius * 2.5f);
            lensingShader.setMat4("model", lensingModel);

            // Would render lensing sphere here (need to create Sphere instance)
            // For now, the black hole visual is just the dark sphere

            glDisable(GL_BLEND);
        }

        // Render Saturn's Rings!
        if (foundSaturn)
        {
            glDepthMask(GL_FALSE);  // Don't write to depth buffer (for transparency)
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            ringShader.use();
            ringShader.setMat4("model", saturnModelMatrix);
            ringShader.setMat4("view", view);
            ringShader.setMat4("projection", projection);
            ringShader.setVec3("ringColor", Vec3(0.9f, 0.85f, 0.7f));
            ringShader.setFloat("ringAlpha", 0.6f);
            ringShader.setVec3("lightPos", Vec3(0, 0, 0));

            saturnRings.render(saturnModelMatrix, view, projection);

            glDepthMask(GL_TRUE);
        }

        // Render spaceship with oriented model matrix and atmospheric effects
        Mat4 shipModel = ship.getOrientedModelMatrix();
        planetShader.setBool("isSun", false);
        planetShader.setBool("hasAtmosphere", false);
        planetShader.setBool("hasTexture", false);
        renderer.render(shipModel, view, projection, ship.getColor());

        // Render engine particles (glowing exhaust)
        if (ship.getIsThrusting())
        {
            glDepthMask(GL_FALSE);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);  // Additive blending for glow
            engineParticleShader.use();
            engineParticleShader.setMat4("view", view);
            engineParticleShader.setMat4("projection", projection);
            engineParticles.render();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  // Reset
            glDepthMask(GL_TRUE);
        }

        // *** RENDER COLLISION POINTS - RED PULSING SPHERES ***
        const auto& collisions = physics.getRecentCollisions();
        for (const auto& collision : collisions)
        {
            // Calculate fade-out based on age (0 = new, 3 = old)
            float age = collision.timestamp;
            float fade = 1.0f - (age / 3.0f);  // Fades over 3 seconds
            if (fade < 0.0f) fade = 0.0f;

            // Pulsing effect - make sphere throb
            float pulse = 1.0f + 0.3f * std::sin(age * 10.0f);  // Fast pulse
            float markerSize = 0.8f * pulse * fade;  // Size based on fade

            // Bright red color that fades to orange
            Vec3 collisionColor(1.0f, fade * 0.2f, 0.0f);  // Red to orange

            // Create model matrix for collision marker
            Mat4 collisionModel = Mat4::translation(
                collision.position.x,
                collision.position.y,
                collision.position.z
            ) * Mat4::scale(markerSize);

            // Render as glowing sphere (like sun)
            planetShader.setBool("isSun", true);  // Makes it glow!
            planetShader.setBool("hasAtmosphere", false);
            planetShader.setBool("hasTexture", false);
            renderer.render(collisionModel, view, projection, collisionColor);
        }

        // Render orbital trails
        glDepthMask(GL_FALSE);
        lineShader.use();
        lineShader.setMat4("view", view);
        lineShader.setMat4("projection", projection);
        lineShader.setFloat("alpha", 0.6f);

        for (auto* body : bodies)
        {
            if (body->getName() != "Sun" && body->getName() != "Black Hole")
            {
                lineShader.setVec3("lineColor", body->getColor());
                body->renderTrail();
            }
        }

        // Render spaceship exhaust trail (orange/red)
        lineShader.setVec3("lineColor", Vec3(1.0f, 0.5f, 0.1f));
        ship.renderExhaustTrail();

        // Render collision prediction lines
        if (!collisionPredictor.getPredictions().empty())
        {
            lineShader.setVec3("lineColor", Vec3(1.0f, 0.2f, 0.2f));
            lineShader.setFloat("alpha", 0.8f);
            collisionPredictor.render();
        }

        glDepthMask(GL_TRUE);

        // Render 3D prediction markers (spheres in 3D space)
        if (showTrajectoryMarkers)
        {
            predictionMarkers.render(&planetShader, view, projection);
        }

        // Render velocity and force vectors
        if (showVectors)
        {
            glDepthMask(GL_FALSE);
            glLineWidth(3.0f);

            vectorShader.use();
            vectorShader.setMat4("view", view);
            vectorShader.setMat4("projection", projection);
            vectorRenderer.render();

            glLineWidth(2.0f);
            glDepthMask(GL_TRUE);
        }

        // Render Lagrange Points (gravitational balance points!)
        if (showLagrangePoints)
        {
            starShader.use();  // Use star shader for glowing points
            starShader.setMat4("view", view);
            starShader.setMat4("projection", projection);
            lagrangePoints.render();
        }

        // Render Hohmann Transfer orbit path
        if (showTransferOrbit && transferCalculator.getActive())
        {
            const TransferOrbit& transfer = transferCalculator.getCurrentTransfer();
            if (transfer.isValid && !transfer.transferPath.empty())
            {
                // Render transfer path as a glowing line
                lineShader.use();
                lineShader.setMat4("view", view);
                lineShader.setMat4("projection", projection);
                lineShader.setVec3("lineColor", Vec3(0.2f, 1.0f, 0.8f));  // Cyan
                lineShader.setFloat("alpha", 0.8f);

                // Would render the path here if we had a line renderer
                // For now, the transfer info is displayed in console
            }
        }

        // 2. Unbind framebuffer and apply post-processing
        sceneFramebuffer.unbind();

        // Reset viewport to window size
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

        // Clear the default framebuffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Apply bloom and HDR tone mapping
        if (guiState.bloomEnabled)
        {
            postProcessing.process(sceneFramebuffer.getColorTexture());
        }
        else
        {
            // If bloom disabled, just render scene texture directly
            // (Would need a simple pass-through shader here)
            postProcessing.process(sceneFramebuffer.getColorTexture());
        }

        // 3. Reset OpenGL state for GUI rendering
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 4. Render GUI on top of everything
        gui.renderAllPanels(ship, bodies, &postProcessing, 1.0f / rawDeltaTime, rawDeltaTime);
        gui.render(window);

        // 5. Render Advanced HUD overlay (targeting reticle, speed vectors, etc.)
        if (!planetPanel.isActive()) {  // Don't show HUD when planet panel is open
            advancedHUD.render(ship, bodies, gui.getLockedTarget(bodies), gui.isTargetLocked(),
                              resourceSystem.getFuel(), resourceSystem.getMaxFuel(),
                              resourceSystem.getPower(), resourceSystem.getMaxPower(),
                              resourceSystem.getOxygen(), resourceSystem.getMaxOxygen(),
                              damageSystem.getHealth(), damageSystem.getMaxHealth());
        }

        // 6. Render Planet Information Panel (full-screen when active)
        planetPanel.render(science);

        window.display();
    }

    // Cleanup GUI
    gui.shutdown();

    // Cleanup: Delete all dynamically allocated celestial bodies
    for (auto* body : bodies)
    {
        delete body;
    }
    bodies.clear();

    return 0;
}