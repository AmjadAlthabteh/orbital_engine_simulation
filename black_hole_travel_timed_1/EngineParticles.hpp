#pragma once
#include "Vec3.hpp"
#include <vector>
#include <glad/glad.h>

// Individual particle for engine exhaust
struct Particle
{
    Vec3 position;
    Vec3 velocity;
    Vec3 color;
    float size;
    float lifetime;
    float maxLifetime;
    float alpha;
};

// Particle system for spaceship engine exhaust effects
class EngineParticles
{
private:
    std::vector<Particle> particles;
    unsigned int VAO, VBO;
    std::vector<float> vertexData;
    int maxParticles;

public:
    EngineParticles(int maxCount = 500);
    ~EngineParticles();

    // Emit particles from engine position in opposite direction of thrust
    void emit(const Vec3& position, const Vec3& direction, const Vec3& shipVelocity,
              float intensity, int count = 5);

    void update(float deltaTime);
    void render();

private:
    void setupBuffers();
    void updateBuffers();
};
