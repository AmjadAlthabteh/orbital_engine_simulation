
// Advanced 3D Solar System Simulation with Orbital Mechanics
// Features: Realistic gravity, collision prediction, orbital trails, starfield
// Visual Physics: Velocity vectors, force vectors, prediction markers, trajectory points
#include <SFML/Window.hpp>
#include <glad/glad.h>
#include <iostream>
#include <cmath>

#include "Camera.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include "SolarSystemFactory.hpp"
#include "PhysicsEngine.hpp"
#include "StarField.hpp"
#include "Trail.hpp"
#include "CollisionPredictor.hpp"
#include "OrbitalPath.hpp"
#include "PredictionMarker.hpp"
#include "VectorRenderer.hpp"
#include "Spaceship.hpp"

const char* vertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalize(aPos);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool isSun;

void main()
{
    if (isSun)
    {
        // Sun emits light with glow effect
        vec3 glow = objectColor * 1.3;
        FragColor = vec4(glow, 1.0);
    }
    else
    {
        // Ambient lighting
        vec3 ambient = 0.15 * objectColor;

        // Diffuse lighting
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * objectColor;

        // Specular lighting
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        vec3 specular = 0.3 * spec * vec3(1.0, 1.0, 1.0);

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }
}
)";

// Line shader for trails and collision predictions
const char* lineVertexShaderSource = R"(
#version 460 core
layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
}
)";

const char* lineFragmentShaderSource = R"(
#version 460 core
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
#version 460 core
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
#version 460 core
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
#version 460 core
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
#version 460 core
out vec4 FragColor;

in vec3 vertexColor;

void main()
{
    FragColor = vec4(vertexColor, 0.9);
}
)";

int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.majorVersion = 4;
    settings.minorVersion = 6;

    sf::Window window(
        sf::VideoMode(1280, 720),
        "Black Hole Engine",
        sf::Style::Default,
        settings
    );

    window.setVerticalSyncEnabled(true);
    window.setMouseCursorGrabbed(true);
    window.setMouseCursorVisible(false);

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

    // Create shaders
    Shader shader(vertexShaderSource, fragmentShaderSource);
    Shader lineShader(lineVertexShaderSource, lineFragmentShaderSource);
    Shader starShader(starVertexShaderSource, starFragmentShaderSource);
    Shader vectorShader(vectorVertexShaderSource, vectorFragmentShaderSource);

    Renderer renderer(&shader);
    Camera camera;
    camera.position = Vec3(0, 30, 80);

    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║   3D ORBITAL PHYSICS SIMULATOR WITH SPACESHIP             ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n\n";

    std::cout << "┌─ SPACESHIP CONTROLS ──────────────────────────────────────┐\n";
    std::cout << "│  I / ↑ Arrow  │ Thrust forward                            │\n";
    std::cout << "│  K / ↓ Arrow  │ Reverse thrust                            │\n";
    std::cout << "│  J / ← Arrow  │ Rotate left (yaw)                         │\n";
    std::cout << "│  L / → Arrow  │ Rotate right (yaw)                        │\n";
    std::cout << "│  U            │ Pitch up                                  │\n";
    std::cout << "│  O            │ Pitch down                                │\n";
    std::cout << "└───────────────────────────────────────────────────────────┘\n\n";

    std::cout << "┌─ CAMERA CONTROLS ─────────────────────────────────────────┐\n";
    std::cout << "│  WASD         │ Move camera (Free mode only)              │\n";
    std::cout << "│  Mouse        │ Look around (Free mode only)              │\n";
    std::cout << "│  C            │ Toggle mode (Free/Follow/Chase)           │\n";
    std::cout << "└───────────────────────────────────────────────────────────┘\n\n";

    std::cout << "┌─ DISPLAY & TIME CONTROLS ─────────────────────────────────┐\n";
    std::cout << "│  Space        │ Toggle velocity vectors                   │\n";
    std::cout << "│  T            │ Toggle trajectory markers (planets)       │\n";
    std::cout << "│  R            │ Toggle ship trajectory prediction         │\n";
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
    std::cout << "  ✓ Black hole gravity well detection\n\n";

    Mat4 projection = Mat4::perspective(
        45.0f * 0.0174533f,
        1280.0f / 720.0f,
        0.1f,
        2000.0f
    );

    // Initialize solar system
    auto bodies = SolarSystemFactory::createSimpleSystem();
    PhysicsEngine physics(0.1f);

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
                path.calculatePath(body->getPhysicsBody(), *sun, 0.1f, 360);
                path.setColor(body->getColor() * 0.5f);
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

    // Time controls
    float timeScale = 1.0f;      // 1.0 = normal speed
    bool isPaused = false;

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
            if (event.type == sf::Event::Closed)
                window.close();

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
                if (event.key.code == sf::Keyboard::C)
                {
                    camera.toggleMode();
                    std::cout << "Camera mode: " << camera.getModeName() << "\n";
                }
                if (event.key.code == sf::Keyboard::P)
                {
                    isPaused = !isPaused;
                    std::cout << "Simulation: " << (isPaused ? "PAUSED" : "RUNNING") << "\n";
                }
                if (event.key.code == sf::Keyboard::Equal || event.key.code == sf::Keyboard::Add)
                {
                    timeScale *= 1.5f;
                    if (timeScale > 10.0f) timeScale = 10.0f;
                    std::cout << "Time scale: " << timeScale << "x\n";
                }
                if (event.key.code == sf::Keyboard::Hyphen || event.key.code == sf::Keyboard::Subtract)
                {
                    timeScale /= 1.5f;
                    if (timeScale < 0.1f) timeScale = 0.1f;
                    std::cout << "Time scale: " << timeScale << "x\n";
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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::I) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            ship.applyThrust(deltaTime);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::K) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        {
            ship.applyReverseThrust(deltaTime);
        }

        // Rotation controls (J/L or Left/Right arrows for yaw)
        // Use rawDeltaTime for ship controls so they're responsive even when paused/slowed
        float rotationAmount = ship.getRotationSpeed() * rawDeltaTime;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::J) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        {
            ship.rotate(-rotationAmount, 0.0f);  // Rotate left (negative yaw)
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        {
            ship.rotate(rotationAmount, 0.0f);  // Rotate right (positive yaw)
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

        // -------- Physics Update --------
        physics.update(deltaTime);
        for (auto* body : bodies)
            body->update(deltaTime);

        // Update spaceship
        ship.update(deltaTime);

        // *** REAL-TIME DISTANCE/ALTITUDE DISPLAY ***
        static float displayTimer = 0.0f;
        displayTimer += rawDeltaTime;
        if (displayTimer > 1.0f)  // Update every second
        {
            displayTimer = 0.0f;

            // Find nearest celestial body
            float nearestDist = 999999.0f;
            std::string nearestName = "Unknown";
            Vec3 shipPos = ship.getPhysicsBody().position;

            for (auto* body : bodies)
            {
                Vec3 diff = body->getPhysicsBody().position - shipPos;
                float dist = diff.length();
                if (dist < nearestDist)
                {
                    nearestDist = dist;
                    nearestName = body->getName();
                }
            }

            // Calculate altitude (distance from nearest body's surface)
            float altitude = nearestDist;  // Distance from center
            for (auto* body : bodies)
            {
                if (body->getName() == nearestName)
                {
                    altitude = nearestDist - body->getRadius();  // Subtract radius
                    break;
                }
            }

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
            }

            std::cout << "Time scale: " << timeScale << "x " << (isPaused ? "[PAUSED]" : "[RUNNING]") << "\n";
            std::cout << "Camera mode: " << camera.getModeName() << "\n";
            std::cout << "=====================\n";
        }

        // Update collision predictions and markers every 0.5 seconds
        collisionPredictTimer += deltaTime;
        if (collisionPredictTimer > 0.5f)
        {
            std::vector<Body*> physBodies;
            for (auto* body : bodies)
                physBodies.push_back(&body->getPhysicsBody());

            collisionPredictor.predictCollisions(physBodies, 0.1f, 50.0f);

            // Add visual markers for collision points
            predictionMarkers.clear();
            for (const auto& pred : collisionPredictor.getPredictions())
            {
                predictionMarkers.addCollisionMarker(pred.collisionPoint, pred.timeToCollision);
            }

            collisionPredictTimer = 0.0f;
        }

        // Update velocity and force vectors every 0.1 seconds
        vectorUpdateTimer += deltaTime;
        if (vectorUpdateTimer > 0.1f && showVectors)
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

        // Update trajectory prediction points every 1 second
        trajectoryUpdateTimer += deltaTime;
        if (trajectoryUpdateTimer > 1.0f)
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
                            for (size_t i = 0; i < trajectoryPoints.size(); i += 5)
                            {
                                predictionMarkers.addTrajectoryPoint(
                                    trajectoryPoints[i],
                                    body->getColor() * 0.7f
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

                // Add markers for every 3rd point (more frequent for ship)
                if (!shipTrajectoryPoints.empty())
                {
                    for (size_t i = 0; i < shipTrajectoryPoints.size(); i += 3)
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
        glClearColor(0.0f, 0.0f, 0.02f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Mat4 view = camera.getViewMatrix();

        // Render starfield
        starShader.use();
        starShader.setMat4("view", view);
        starShader.setMat4("projection", projection);
        starfield.render();

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

        // Render planets with lighting
        shader.use();
        shader.setVec3("viewPos", camera.position);
        shader.setVec3("lightPos", Vec3(0, 0, 0));

        for (auto* body : bodies)
        {
            Mat4 model = body->getModelMatrix();
            bool isSun = (body->getName() == "Sun");
            shader.setBool("isSun", isSun);
            renderer.render(model, view, projection, body->getColor());
        }

        // Render spaceship with oriented model matrix
        Mat4 shipModel = ship.getOrientedModelMatrix();
        shader.setBool("isSun", false);
        renderer.render(shipModel, view, projection, ship.getColor());

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
            shader.setBool("isSun", true);  // Makes it glow!
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
            predictionMarkers.render(&shader, view, projection);
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

        window.display();
    }

    // Cleanup: Delete all dynamically allocated celestial bodies
    for (auto* body : bodies)
    {
        delete body;
    }
    bodies.clear();

    return 0;
}