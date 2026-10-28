#pragma once
#include <glad/glad.h>
#include <vector>
#include "Vec3.hpp"

// Represents a single star in the galaxy background
struct GalaxyStar
{
    Vec3 position;
    Vec3 color;
    float size;
    float brightness;
    float twinkleSpeed;
    float twinklePhase;
};

// Enhanced galaxy background with realistic star distribution, clusters, and dust lanes
class GalaxyBackground
{
public:
    GalaxyBackground(int starCount = 20000);
    ~GalaxyBackground();

    void update(float deltaTime);
    void render();

    const std::vector<GalaxyStar>& getStars() const;

private:
    void generateGalaxy(int starCount);
    void generateSpiralArm(int starCount, float armAngle, float armWidth);
    void generateGalacticCore(int starCount);
    void generateHaloStars(int starCount);
    void generateDistantGalaxies(int count);

    void setupBuffers();
    void updateBuffers();

    std::vector<GalaxyStar> stars;
    std::vector<float> vertexData;

    GLuint VAO;
    GLuint VBO;

    float animationTime;
};
