#pragma once
#include <glad/glad.h>
#include <vector>
#include <string>
#include "Vec3.hpp"
#include "Body.hpp"
#include "Sphere.hpp"

// Types of Lagrange points
enum class LagrangeType
{
    L1,  // Between two bodies (unstable)
    L2,  // Beyond smaller body (unstable)
    L3,  // Opposite side of larger body (unstable)
    L4,  // 60° ahead of smaller body (stable!)
    L5   // 60° behind smaller body (stable!)
};

// Represents a single Lagrange point
struct LagrangePoint
{
    LagrangeType type;
    Vec3 position;
    Vec3 color;
    float size;
    bool stable;  // L4 and L5 are stable, others are unstable
    std::string label;
};

// Calculates and visualizes Lagrange points between celestial bodies
// These are gravitational "sweet spots" where objects can orbit stably!
// NASA uses these for satellites (James Webb Telescope is at Earth-Sun L2!)
class LagrangePoints
{
public:
    LagrangePoints();
    ~LagrangePoints();

    // Calculate all 5 Lagrange points between two bodies
    void calculatePoints(const Body& primary, const Body& secondary);

    // Update and render
    void update(float deltaTime);
    void render();

    void setVisible(bool visible) { isVisible = visible; }
    bool getVisible() const { return isVisible; }

    const std::vector<LagrangePoint>& getPoints() const { return points; }

private:
    Vec3 calculateL1(const Body& primary, const Body& secondary);
    Vec3 calculateL2(const Body& primary, const Body& secondary);
    Vec3 calculateL3(const Body& primary, const Body& secondary);
    Vec3 calculateL4(const Body& primary, const Body& secondary);
    Vec3 calculateL5(const Body& primary, const Body& secondary);

    void setupBuffers();
    void updateBuffers();

    std::vector<LagrangePoint> points;
    std::vector<float> vertexData;

    Sphere markerSphere;
    GLuint VAO, VBO;
    bool isVisible;
    float animationTime;
};
