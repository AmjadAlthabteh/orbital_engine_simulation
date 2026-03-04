#pragma once
#include "Vec3.hpp"
#include "Sphere.hpp"
#include <vector>
#include <glad/glad.h>

enum class MarkerType
{
    COLLISION_POINT,
    TRAJECTORY_POINT,
    LAGRANGE_POINT,
    PERIAPSIS,
    APOAPSIS
};

struct Marker
{
    Vec3 position;
    Vec3 color;
    float size;
    MarkerType type;
    float lifetime;
    bool active;
};

class PredictionMarker
{
private:
    std::vector<Marker> markers;
    Sphere markerSphere;
    unsigned int lineVAO, lineVBO;
    std::vector<Vec3> lineVertices;

public:
    PredictionMarker();
    ~PredictionMarker();

    void addCollisionMarker(const Vec3& position, float timeToCollision);
    void addTrajectoryPoint(const Vec3& position, const Vec3& color);
    void addLagrangePoint(const Vec3& position);
    void addPeriapsis(const Vec3& position);
    void addApoapsis(const Vec3& position);

    void update(float deltaTime);
    void render(class Shader* shader, const struct Mat4& view, const struct Mat4& projection);
    void clear();

private:
    void setupBuffers();
};
