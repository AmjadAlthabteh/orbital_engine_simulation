#pragma once
#include "Vec3.hpp"
#include <vector>
#include <glad/glad.h>

// Nebula cloud particle for volumetric space background
struct NebulaParticle
{
    Vec3 position;
    Vec3 color;
    float size;
    float density;
    float rotation;
};

// Creates realistic galaxy background with volumetric nebula clouds
class NebulaBackground
{
private:
    std::vector<NebulaParticle> nebulaParticles;
    unsigned int VAO, VBO;
    std::vector<float> vertexData;

    // Nebula regions (multiple colored clouds)
    struct NebulaRegion
    {
        Vec3 center;
        float radius;
        Vec3 color;
        int particleCount;
    };

    std::vector<NebulaRegion> regions;

public:
    NebulaBackground();
    ~NebulaBackground();

    void generateNebula();
    void update(float deltaTime);
    void render();

private:
    void setupBuffers();
    void updateBuffers();
    void createNebulaRegion(const Vec3& center, float radius, const Vec3& color, int particleCount);
};
