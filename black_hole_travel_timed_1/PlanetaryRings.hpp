#pragma once
#include <glad/glad.h>
#include <vector>
#include "Vec3.hpp"
#include "Mat4.hpp"

// Beautiful planetary rings (Saturn, Jupiter, etc.)
class PlanetaryRings
{
public:
    PlanetaryRings(float innerRadius, float outerRadius, int segments = 100);
    ~PlanetaryRings();

    void render(const Mat4& modelMatrix, const Mat4& view, const Mat4& projection);

    void setColor(const Vec3& color) { ringColor = color; }
    void setAlpha(float alpha) { ringAlpha = alpha; }

private:
    void generateRing();
    void setupBuffers();

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    GLuint VAO, VBO, EBO;
    float innerRadius;
    float outerRadius;
    int segments;

    Vec3 ringColor;
    float ringAlpha;
};
