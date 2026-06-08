#pragma once
#include <glad/glad.h>
#include "Vec3.hpp"
#include "Mat4.hpp"
#include "Shader.hpp"
#include <vector>
#include <cmath>

// Accretion disk shader with heat glow and rotation
const char* accretionDiskVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

out vec2 TexCoords;
out float DistFromCenter;

void main()
{
    // Rotate disk over time
    float angle = time * 0.2 + aTexCoords.x * 6.28318;  // Different speeds at different radii
    float cosA = cos(angle);
    float sinA = sin(angle);

    vec3 rotatedPos = aPos;
    float x = aPos.x * cosA - aPos.z * sinA;
    float z = aPos.x * sinA + aPos.z * cosA;
    rotatedPos.x = x;
    rotatedPos.z = z;

    DistFromCenter = length(vec2(aPos.x, aPos.z));
    TexCoords = aTexCoords;

    gl_Position = projection * view * model * vec4(rotatedPos, 1.0);
}
)";

const char* accretionDiskFragmentShader = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in float DistFromCenter;

uniform float time;
uniform float innerRadius;
uniform float outerRadius;

void main()
{
    // Normalize distance (0 = inner, 1 = outer)
    float normalizedDist = (DistFromCenter - innerRadius) / (outerRadius - innerRadius);

    // Fade out at edges
    if (normalizedDist < 0.0 || normalizedDist > 1.0)
        discard;

    // Heat gradient - hotter (white/blue) near center, cooler (red/orange) outside
    vec3 innerColor = vec3(1.0, 0.95, 0.8);   // Hot white-yellow
    vec3 middleColor = vec3(1.0, 0.5, 0.2);   // Orange
    vec3 outerColor = vec3(0.8, 0.2, 0.1);    // Dark red

    vec3 color;
    if (normalizedDist < 0.5)
    {
        color = mix(innerColor, middleColor, normalizedDist * 2.0);
    }
    else
    {
        color = mix(middleColor, outerColor, (normalizedDist - 0.5) * 2.0);
    }

    // Add swirling pattern using noise-like function
    float swirl = sin(normalizedDist * 20.0 + time * 2.0 + TexCoords.x * 10.0) * 0.5 + 0.5;
    color *= (0.7 + swirl * 0.3);

    // Brightness based on distance (brighter near center)
    float brightness = 1.5 - normalizedDist * 0.8;
    color *= brightness;

    // Turbulence effect
    float turbulence = sin(normalizedDist * 30.0 + time * 3.0) * 0.1 + 0.9;
    color *= turbulence;

    // Edge fade
    float edgeFade = smoothstep(0.0, 0.1, normalizedDist) * smoothstep(1.0, 0.9, normalizedDist);

    // Alpha based on distance (more transparent at edges)
    float alpha = 0.8 * edgeFade;

    FragColor = vec4(color, alpha);
}
)";

class AccretionDisk
{
private:
    GLuint VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    int indexCount;
    float innerRadius;
    float outerRadius;
    float time;

public:
    AccretionDisk(float inner, float outer, int segments = 100, int rings = 30)
        : innerRadius(inner), outerRadius(outer), time(0.0f)
    {
        generateDiskMesh(segments, rings);
        setupBuffers();
    }

    void generateDiskMesh(int segments, int rings)
    {
        vertices.reserve(static_cast<size_t>(rings + 1) * static_cast<size_t>(segments + 1) * 5);
        indices.reserve(static_cast<size_t>(rings) * static_cast<size_t>(segments) * 6);

        // Generate ring vertices
        for (int r = 0; r <= rings; r++)
        {
            float ringFactor = (float)r / rings;
            float radius = innerRadius + (outerRadius - innerRadius) * ringFactor;

            for (int s = 0; s <= segments; s++)
            {
                float angle = (float)s / segments * 2.0f * 3.14159265359f;
                float x = cos(angle) * radius;
                float z = sin(angle) * radius;
                float y = 0.0f;  // Flat disk

                // Position
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                // Texture coordinates
                vertices.push_back((float)s / segments);
                vertices.push_back((float)r / rings);
            }
        }

        // Generate indices
        for (int r = 0; r < rings; r++)
        {
            for (int s = 0; s < segments; s++)
            {
                int current = r * (segments + 1) + s;
                int next = current + segments + 1;

                // Two triangles per quad
                indices.push_back(current);
                indices.push_back(next);
                indices.push_back(current + 1);

                indices.push_back(current + 1);
                indices.push_back(next);
                indices.push_back(next + 1);
            }
        }

        indexCount = indices.size();
    }

    void setupBuffers()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Texture coordinate attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void update(float deltaTime)
    {
        time += deltaTime;
    }

    void render(Shader* shader, const Mat4& model, const Mat4& view, const Mat4& projection)
    {
        shader->use();
        shader->setMat4("model", model);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setFloat("time", time);
        shader->setFloat("innerRadius", innerRadius);
        shader->setFloat("outerRadius", outerRadius);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    ~AccretionDisk()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }
};
