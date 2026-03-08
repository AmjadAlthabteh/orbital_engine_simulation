#include "PlanetaryRings.hpp"
#include <cmath>

const float PI = 3.14159265359f;

PlanetaryRings::PlanetaryRings(float innerRadius, float outerRadius, int segments)
    : innerRadius(innerRadius), outerRadius(outerRadius), segments(segments),
      ringColor(Vec3(0.9f, 0.85f, 0.7f)), ringAlpha(0.7f)
{
    generateRing();
    setupBuffers();
}

PlanetaryRings::~PlanetaryRings()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void PlanetaryRings::generateRing()
{
    vertices.clear();
    indices.clear();

    // Generate ring vertices (flat disk in XZ plane)
    for (int i = 0; i <= segments; i++)
    {
        float angle = (float)i / (float)segments * 2.0f * PI;
        float cosAngle = std::cos(angle);
        float sinAngle = std::sin(angle);

        // Inner vertex
        float x_inner = innerRadius * cosAngle;
        float z_inner = innerRadius * sinAngle;
        vertices.push_back(x_inner);
        vertices.push_back(0.0f);  // Y = 0 (flat ring)
        vertices.push_back(z_inner);

        // Normal (pointing up)
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);

        // Texture coordinate (inner)
        vertices.push_back(0.0f);
        vertices.push_back((float)i / (float)segments);

        // Outer vertex
        float x_outer = outerRadius * cosAngle;
        float z_outer = outerRadius * sinAngle;
        vertices.push_back(x_outer);
        vertices.push_back(0.0f);
        vertices.push_back(z_outer);

        // Normal (pointing up)
        vertices.push_back(0.0f);
        vertices.push_back(1.0f);
        vertices.push_back(0.0f);

        // Texture coordinate (outer)
        vertices.push_back(1.0f);
        vertices.push_back((float)i / (float)segments);
    }

    // Generate indices for triangle strip
    for (int i = 0; i < segments; i++)
    {
        int base = i * 2;

        // First triangle
        indices.push_back(base);
        indices.push_back(base + 1);
        indices.push_back(base + 2);

        // Second triangle
        indices.push_back(base + 1);
        indices.push_back(base + 3);
        indices.push_back(base + 2);
    }
}

void PlanetaryRings::setupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void PlanetaryRings::render(const Mat4& modelMatrix, const Mat4& view, const Mat4& projection)
{
    // Render logic will be handled by shader in main.cpp
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
