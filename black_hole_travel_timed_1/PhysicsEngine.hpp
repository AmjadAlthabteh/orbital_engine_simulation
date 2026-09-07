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

struct PhysicsDiagnostics
{
    float kineticEnergy = 0.0f;
    float potentialEnergy = 0.0f;
    float totalEnergy = 0.0f;
    Vec3 totalMomentum;
    float closestApproach = 0.0f;
};

class PhysicsEngine
{
private:
    std::vector<Body*> bodies;
    float gravitationalConstant;
    std::vector<CollisionEvent> recentCollisions;  // Track recent collisions
    PhysicsDiagnostics diagnostics;

    // Adaptive substepping parameters for improved stability
    float maxTimeStep;         // Maximum timestep before subdivision (default 0.016f)
    int maxSubsteps;           // Maximum substeps per frame (default 8)
    float velocityThreshold;   // Velocity threshold for adaptive stepping (default 100.0f)
    float gravitySofteningLength;  // Prevent singular forces during very close passes
    float maxVelocityChangePerStep;  // Acceleration-based substep threshold

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
    void setMaxTimeStep(float maxStep);
    void setMaxSubsteps(int maxSteps);
    void setVelocityThreshold(float threshold);
    void setGravitySofteningLength(float length);
    void setMaxVelocityChangePerStep(float deltaV);
    int getLastSubstepCount() const { return lastSubstepCount; }
    const PhysicsDiagnostics& getDiagnostics() const { return diagnostics; }

private:
    void applyGravity();
    void handleCollisions();
    void updateDiagnostics();
    bool needsAdaptiveStepping() const;  // Check if adaptive stepping is needed
    int calculateAdaptiveSubsteps(float deltaTime) const;
    mutable int lastSubstepCount;        // Track substeps for diagnostics
};
