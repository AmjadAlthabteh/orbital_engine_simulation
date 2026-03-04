
// Advanced 3D Solar System Simulation with Orbital Mechanics
// Features: Realistic gravity, collision prediction, orbital trails, starfield
// Visual Physics: Velocity vectors, force vectors, prediction markers, trajectory points
#include <SFML/Window.hpp>
#include <glad/glad.h>
#include <iostream>

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

    std::cout << "=== 3D ORBITAL PHYSICS SIMULATOR ===\n";
    std::cout << "Controls:\n";
    std::cout << "  WASD - Move camera\n";
    std::cout << "  Mouse - Look around\n";
    std::cout << "  Space - Toggle vector display\n";
    std::cout << "  T - Toggle trajectory markers\n";
    std::cout << "  ESC - Exit\n\n";

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

    sf::Clock clock;
    float collisionPredictTimer = 0.0f;
    float vectorUpdateTimer = 0.0f;
    float trajectoryUpdateTimer = 0.0f;

    float lastX = 640;
    float lastY = 360;
    bool firstMouse = true;

    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();

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
                if (event.key.code == sf::Keyboard::Escape)
                {
                    window.close();
                }
            }
        }

        // -------- Mouse Look --------
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

        // -------- WASD Movement --------
        float speed = 20.0f * deltaTime;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            camera.position = camera.position + camera.front * speed;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            camera.position = camera.position - camera.front * speed;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            camera.position = camera.position - camera.right * speed;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            camera.position = camera.position + camera.right * speed;

        // -------- Physics Update --------
        physics.update(deltaTime);
        for (auto* body : bodies)
            body->update(deltaTime);

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
            vectorUpdateTimer = 0.0f;
        }

        // Update trajectory prediction points every 1 second
        trajectoryUpdateTimer += deltaTime;
        if (trajectoryUpdateTimer > 1.0f && showTrajectoryMarkers)
        {
            // Calculate and show trajectory points for selected bodies
            std::vector<Body*> allBodies;
            for (auto* body : bodies)
                allBodies.push_back(&body->getPhysicsBody());

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
                    for (size_t i = 0; i < trajectoryPoints.size(); i += 5)
                    {
                        predictionMarkers.addTrajectoryPoint(
                            trajectoryPoints[i],
                            body->getColor() * 0.7f
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

    return 0;
}