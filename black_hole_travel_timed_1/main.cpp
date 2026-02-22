
// spacehsip and more like mor epn prject based -- enhance two projects 
// Pleas elie in doeth program for the spaceship and liek do math and more 
#include <SFML/Window.hpp>
#include <glad/glad.h>
#include <iostream>
#include <vector>

#include "Mat4.hpp"
#include "Vec3.hpp"
#include "Shader.hpp"

int main()
{
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.majorVersion = 4;
    settings.minorVersion = 6;

    sf::Window window(
        sf::VideoMode(1280, 720),
        "Cube Test",
        sf::Style::Default,
        settings
    );

    if (!gladLoadGL())
    {
        std::cout << "GLAD failed\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    const char* vertexShaderSrc = R"(
    #version 460 core
    layout(location = 0) in vec3 aPos;

    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;

    void main()
    {
        gl_Position = projection * view * model * vec4(aPos,1.0);
    }
    )";

    const char* fragmentShaderSrc = R"(
    #version 460 core
    out vec4 FragColor;
    uniform vec3 objectColor;

    void main()
    {
        FragColor = vec4(objectColor,1.0);
    }
    )";

    Shader shader(vertexShaderSrc, fragmentShaderSrc);

    float vertices[] = {
        -1,-1,-1,  1,-1,-1,  1, 1,-1,
         1, 1,-1, -1, 1,-1, -1,-1,-1,

        -1,-1, 1,  1,-1, 1,  1, 1, 1,
         1, 1, 1, -1, 1, 1, -1,-1, 1,

        -1, 1, 1, -1, 1,-1, -1,-1,-1,
        -1,-1,-1, -1,-1, 1, -1, 1, 1,

         1, 1, 1,  1, 1,-1,  1,-1,-1,
         1,-1,-1,  1,-1, 1,  1, 1, 1,

        -1,-1,-1,  1,-1,-1,  1,-1, 1,
         1,-1, 1, -1,-1, 1, -1,-1,-1,

        -1, 1,-1,  1, 1,-1,  1, 1, 1,
         1, 1, 1, -1, 1, 1, -1, 1,-1
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        Mat4 projection = Mat4::perspective(
            45.f * 3.14159f / 180.f,
            1280.f / 720.f,
            0.1f,
            100.f
        );

        Mat4 view = Mat4::translate(Vec3(0, 0, -6));
        Mat4 model = Mat4::identity();

        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", model);

        shader.setVec3("objectColor", 0.2f, 0.6f, 1.0f);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        window.display();
    }

    return 0;
}