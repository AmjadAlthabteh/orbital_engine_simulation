#pragma once
#include "Vec3.hpp"
#include <vector>
#include <glad/glad.h>

struct VectorArrow
{
    Vec3 origin;
    Vec3 direction;
    Vec3 color;
    float magnitude;
    bool active;
};

class VectorRenderer
{
private:
    std::vector<VectorArrow> velocityVectors;
    std::vector<VectorArrow> forceVectors;
    unsigned int VAO, VBO;
    std::vector<float> vertexData;

public:
    VectorRenderer();
    ~VectorRenderer();

    void addVelocityVector(const Vec3& origin, const Vec3& velocity);
    void addForceVector(const Vec3& origin, const Vec3& force);
    void clear();
    void render();

private:
    void setupBuffers();
    void updateBuffers();
    void createArrowGeometry(const Vec3& origin, const Vec3& direction, float magnitude, const Vec3& color);
};
