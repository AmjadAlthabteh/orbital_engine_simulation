#pragma once
#include "Vec3.hpp"
#include "Body.hpp"
#include <vector>
#include <glad/glad.h>

class OrbitalPath
{
private:
    std::vector<Vec3> pathPoints;
    Vec3 color;
    unsigned int VAO, VBO;

public:
    OrbitalPath();
    ~OrbitalPath();

    void calculatePath(const Body& body, const Body& centralBody, float G, int numPoints = 360);
    void render();
    void setColor(const Vec3& newColor);

private:
    void setupBuffers();
    void updateBuffers();
};
