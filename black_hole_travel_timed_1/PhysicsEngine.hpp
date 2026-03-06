#pragma once
#include "Body.hpp"
#include "Vec3.hpp"
#include <vector>

struct CollisionEvent
{
    Vec3 position;        // Where collision happened
    float timestamp;      // When it happened (for fade-out)
    float intensity;      // How hard (for size/color)
    Body* bodyA;          // What collided
    Body* bodyB;
};

class PhysicsEngine
{
private:
    std::vector<Body*> bodies;
    float gravitationalConstant;
    std::vector<CollisionEvent> recentCollisions;  // Track recent collisions

public:
    PhysicsEngine(float G = 0.1f);

    void addBody(Body* body);
    void update(float deltaTime);
    bool checkCollision(const Body* a, const Body* b) const;
    float predictCollisionTime(const Body* a, const Body* b) const;

    // Collision tracking
    const std::vector<CollisionEvent>& getRecentCollisions() const;
    void updateCollisionMarkers(float deltaTime);  // Fade out old collisions

private:
    void applyGravity();
    void handleCollisions();
};