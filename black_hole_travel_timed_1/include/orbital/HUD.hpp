#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>
#include "Vec3.hpp"
#include "Spaceship.hpp"
#include "CelestialBody.hpp"

// Simple 2D HUD rendering system for displaying physics data and UI elements
class HUD
{
private:
    unsigned int quadVAO, quadVBO;
    unsigned int textureID;

    // HUD rendering data
    struct HUDElement
    {
        float x, y;          // Screen position (-1 to 1)
        float width, height;  // Size
        Vec3 color;          // Color
        float alpha;         // Transparency
    };

    std::vector<HUDElement> elements;

public:
    HUD();
    ~HUD();

    // Main rendering function
    void render(const Spaceship& ship,
                const std::vector<CelestialBody*>& bodies,
                float timeScale,
                bool isPaused,
                const std::string& cameraMode);

    // Individual HUD components
    void renderSpeedometer(float speed, float maxSpeed);
    void renderAltimeter(float altitude, const std::string& nearestBody);
    void renderCompass(const Vec3& shipPos, const std::vector<CelestialBody*>& bodies);
    void renderPhysicsData(const Spaceship& ship);
    void renderOrbitalParameters(const Spaceship& ship, const Vec3& centralBodyPos, float centralBodyMass);
    void renderMiniMap(const Spaceship& ship, const std::vector<CelestialBody*>& bodies);
    void renderCrosshair();
    void renderStatusBar(float timeScale, bool isPaused, const std::string& cameraMode);

    // Utility functions
    void renderBox(float x, float y, float width, float height, const Vec3& color, float alpha);
    void renderLine(float x1, float y1, float x2, float y2, const Vec3& color, float alpha);
    void renderCircle(float x, float y, float radius, const Vec3& color, float alpha, int segments = 32);

private:
    void setupQuad();
    void calculateOrbitalElements(const Vec3& pos, const Vec3& vel, const Vec3& centralPos,
                                  float centralMass, float& apoapsis, float& periapsis,
                                  float& eccentricity, float& period);
};
