
// spacehsip and more like mor epn prject based -- enhance two projects 
// Pleas elie in doeth program for the spaceship and liek do math and more 
#include <SFML/Window.hpp>
#include <glad/glad.h>
#include <iostream>

#include "Camera.hpp"
#include "Shader.hpp"
#include "Renderer.hpp"
#include "SolarSystemFactory.hpp"
#include "PhysicsEngine.hpp"

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
        // Sun emits light, no lighting calculation
        FragColor = vec4(objectColor, 1.0);
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

    Shader shader(vertexShaderSource, fragmentShaderSource);
    Renderer renderer(&shader);
    Camera camera;

    Mat4 projection = Mat4::perspective(
        45.0f * 0.0174533f,
        1280.0f / 720.0f,
        0.1f,
        1000.0f
    );

    auto bodies = SolarSystemFactory::createSimpleSystem();
    PhysicsEngine physics(0.1f);

    for (auto* body : bodies)
        physics.addBody(&body->getPhysicsBody());

    sf::Clock clock;

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

        // -------- Physics --------
        physics.update(deltaTime);
        for (auto* body : bodies)
            body->update(deltaTime);

        glClearColor(0.0f, 0.0f, 0.02f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Mat4 view = camera.getViewMatrix();

        shader.use();
        shader.setVec3("viewPos", camera.position);
        shader.setVec3("lightPos", Vec3(0, 0, 0)); // Sun at origin

        for (auto* body : bodies)
        {
            Mat4 model = body->getModelMatrix();
            bool isSun = (body->getName() == "Sun");
            shader.setBool("isSun", isSun);
            renderer.render(model, view, projection, body->getColor());
        }

        window.display();
    }

    return 0;
}