#pragma once

#include "Entity.hpp"
#include "Body.hpp"
#include "Trail.hpp"
#include <memory>

class Spaceship : public Entity
{
private:
    // Physics body for gravity and collision
    Body physicsBody;

    // Spaceship properties
    float radius;
    Vec3 color;

    // Thrust system (unlimited fuel mode)
    float thrustPower;          // Thrust force magnitude (e.g., 50.0f)
    bool isThrusting;           // Currently applying thrust?

    // Rotation/Orientation
    float yaw;                  // Rotation around Y axis (degrees)
    float pitch;                // Rotation around X axis (degrees)
    float rotationSpeed;        // Rotation rate (degrees per second)
    Vec3 forward;               // Forward direction vector
    Vec3 up;                    // Up direction vector
    Vec3 right;                 // Right direction vector

    // Visual effects
    std::unique_ptr<Trail> exhaustTrail;
    bool hasExhaustTrail;
    float exhaustTimer;
    float exhaustInterval;      // Time between exhaust trail points

public:
    // Constructor
    Spaceship(float mass = 1.0f, float radius = 0.5f, const Vec3& color = Vec3(0.2f, 0.8f, 1.0f));

    // Entity interface
    void update(float deltaTime) override;

    // Control methods
    void applyThrust(float deltaTime);      // Apply forward thrust
    void applyReverseThrust(float deltaTime); // Apply reverse thrust
    void stopThrust();                      // Stop thrusting
    void rotate(float yawDelta, float pitchDelta); // Rotate ship
    void updateDirectionVectors();          // Update forward/up/right from yaw/pitch

    // Physics integration
    Body& getPhysicsBody();
    const Body& getPhysicsBody() const;

    // State queries
    float getSpeed() const;                 // Current speed magnitude
    Vec3 getVelocity() const;               // Current velocity vector
    Vec3 getForward() const;                // Forward direction
    Vec3 getUp() const;                     // Up direction
    Vec3 getRight() const;                  // Right direction
    float getYaw() const;                   // Current yaw angle
    float getPitch() const;                 // Current pitch angle
    bool getIsThrusting() const;            // Is ship currently thrusting?
    float getRotationSpeed() const;         // Get rotation speed

    // Visual properties
    const Vec3& getColor() const;
    float getRadius() const;
    Mat4 getOrientedModelMatrix() const;    // Model matrix with rotation

    // Trail system
    void enableExhaustTrail(size_t maxPoints = 300);
    void renderExhaustTrail();

    // Utility
    void setThrustPower(float power);       // Adjust thrust strength
    void setRotationSpeed(float speed);     // Adjust rotation rate
};
