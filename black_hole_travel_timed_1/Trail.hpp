#pragma once
#include "Vec3.hpp"
#include <vector>
#include <glad/glad.h>

class Trail
{
private:
    std::vector<Vec3> positions;
    size_t maxPoints;
    Vec3 color;

    unsigned int VAO, VBO;
    bool needsUpdate;

public:
    Trail(size_t maxPoints = 500, const Vec3& color = Vec3(1.0f, 1.0f, 1.0f));
    ~Trail();

    void addPoint(const Vec3& position);
    void clear();
    void render();
    void setColor(const Vec3& newColor);

private:
    void setupBuffers();
    void updateBuffers();
};
