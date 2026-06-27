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

    // Adaptive substepping parameters for improved stability
    float maxTimeStep;         // Maximum timestep before subdivision (default 0.016f)
    int maxSubsteps;           // Maximum substeps per frame (default 8)
    float velocityThreshold;   // Velocity threshold for adaptive stepping (default 100.0f)
    float gravitySofteningLength;  // Prevent singular forces during very close passes

public:
    PhysicsEngine(float G = 0.1f);

    void addBody(Body* body);
    void update(float deltaTime);
    bool checkCollision(const Body* a, const Body* b) const;
    float predictCollisionTime(const Body* a, const Body* b) const;

    // Collision tracking
    const std::vector<CollisionEvent>& getRecentCollisions() const;
    void updateCollisionMarkers(float deltaTime);  // Fade out old collisions

    // Adaptive substepping configuration
    void setMaxTimeStep(float maxStep) { maxTimeStep = maxStep; }
    void setMaxSubsteps(int maxSteps) { maxSubsteps = maxSteps; }
    void setVelocityThreshold(float threshold) { velocityThreshold = threshold; }
    void setGravitySofteningLength(float length) { gravitySofteningLength = length; }
    int getLastSubstepCount() const { return lastSubstepCount; }

private:
    void applyGravity();
    void handleCollisions();
    bool needsAdaptiveStepping() const;  // Check if adaptive stepping is needed
    int calculateAdaptiveSubsteps(float deltaTime) const;
    mutable int lastSubstepCount;        // Track substeps for diagnostics
};
