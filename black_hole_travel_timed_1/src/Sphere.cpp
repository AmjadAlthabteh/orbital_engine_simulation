#include "Sphere.hpp"
#include <glad/glad.h>
#include <cmath>
#include <cstddef>
#include <vector>

Sphere::Sphere(float radius, int sectors, int stacks)
    : sectorCount(sectors), stackCount(stacks)
{
    constexpr float pi = 3.1415926f;
    constexpr float twoPi = 2.0f * pi;
    const float stackStep = pi / static_cast<float>(stacks);
    const float sectorStep = twoPi / static_cast<float>(sectors);
    const float invStacks = 1.0f / static_cast<float>(stacks);
    const float invSectors = 1.0f / static_cast<float>(sectors);
    const float invRadius = 1.0f / radius;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    vertices.reserve(static_cast<size_t>(stacks + 1) * static_cast<size_t>(sectors + 1) * 8);
    if (stacks > 1 && sectors > 0)
    {
        indices.reserve(static_cast<size_t>(stacks - 1) * static_cast<size_t>(sectors) * 6);
    }

    for (int i = 0; i <= stacks; ++i)
    {
        float stackAngle = pi / 2.0f - static_cast<float>(i) * stackStep;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j)
        {
            float sectorAngle = static_cast<float>(j) * sectorStep;

            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            // POSITION
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // TEXTURE COORDINATES (UV mapping)
            float u = static_cast<float>(j) * invSectors;
            float v = static_cast<float>(i) * invStacks;
            vertices.push_back(u);
            vertices.push_back(v);

            // NORMAL (important for lighting)
            vertices.push_back(x * invRadius);
            vertices.push_back(y * invRadius);
            vertices.push_back(z * invRadius);
        }
    }

    for (int i = 0; i < stacks; ++i)
    {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stacks - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    indexCount = indices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW);

    // POSITION (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        8 * sizeof(float),
        (void*)0);
    glEnableVertexAttribArray(0);

    // TEXTURE COORDS (location 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
        8 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // NORMAL (location 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
        8 * sizeof(float),
        (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Sphere::draw() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

// OPTIMIZATION: Get sector/stack counts for different LOD levels
void Sphere::getLODCounts(SphereLOD lod, int& outSectors, int& outStacks)
{
    switch (lod)
    {
        case SphereLOD::HIGH:
            outSectors = 36;
            outStacks = 18;
            break;
        case SphereLOD::MEDIUM:
            outSectors = 24;
            outStacks = 12;
            break;
        case SphereLOD::LOW:
            outSectors = 12;
            outStacks = 6;
            break;
        default:
            outSectors = 24;
            outStacks = 12;
    }
} 
