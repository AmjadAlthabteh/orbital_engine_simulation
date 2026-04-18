#pragma once

#include "Entity.hpp"
#include "Body.hpp"
#include "Trail.hpp"
#include <memory>
#include <string>

// Forward declaration
class CelestialBody;

// Landing state machine
enum class LandingState
{
    FLYING,       // Normal flight mode
    APPROACHING,  // Within landing range, velocity low enough
    LANDED        // Landed on surface, physics disabled
};

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
    float roll;                 // BARREL ROLL! Rotation around Z axis (degrees)
    float rotationSpeed;        // Rotation rate (degrees per second)
    Vec3 forward;               // Forward direction vector
    Vec3 up;                    // Up direction vector
    Vec3 right;                 // Right direction vector

    // CRAZY BARREL ROLL SYSTEM!
    bool isBarrelRolling;       // Currently doing a sick barrel roll
    float barrelRollProgress;   // Progress through the roll (0.0 to 360.0)
    float barrelRollSpeed;      // How fast we spin (degrees per second)

    // Auto-stabilization system
    bool autoStabilizationEnabled;  // Auto-dampen rotation when no input
    float angularVelocityYaw;       // Current yaw rotation velocity
    float angularVelocityPitch;     // Current pitch rotation velocity
    float rotationDamping;          // Damping factor (0.0 to 1.0)

    // Speed limiter system
    bool speedLimiterEnabled;       // Cap maximum speed
    float maxSpeedLimit;            // Maximum allowed speed

    // TURBO BOOST SYSTEM - GOTTA GO FAST!
    bool isTurboBoostActive;        // Is turbo boost engaged?
    float turboBoostMultiplier;     // Thrust multiplier during boost (3x!)
    float turboBoostDuration;       // How long boost lasts
    float turboBoostTimer;          // Current boost timer
    float turboBoostCooldown;       // Cooldown before next boost
    float turboBoostCooldownTimer;  // Current cooldown timer

    // Visual effects
    std::unique_ptr<Trail> exhaustTrail;
    bool hasExhaustTrail;
    float exhaustTimer;
    float exhaustInterval;      // Time between exhaust trail points
    bool rainbowExhaustMode;    // RAINBOW POWER MODE!
    float exhaustColorPhase;    // Color animation phase

    // Landing system
    LandingState landingState;
    CelestialBody* landedOn;    // Which body we're landed on (nullptr if flying)
    Vec3 landingOffset;         // Offset from planet surface when landed
    float landingDistThreshold; // How close to land (2.0 * radius sum)
    float landingVelThreshold;  // Max velocity to land (5.0 units/s)

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

    // CRAZY NEW FEATURES!
    void initiateBarrelRoll();              // DO A BARREL ROLL!
    void updateBarrelRoll(float deltaTime); // Update barrel roll animation
    void activateTurboBoost();              // ENGAGE TURBO BOOST!
    void updateTurboBoost(float deltaTime); // Update boost timer
    void toggleRainbowExhaust();            // Toggle rainbow exhaust mode
    Vec3 getRainbowColor();                 // Get current rainbow color

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

    // Auto-stabilization system
    void toggleAutoStabilization();         // Toggle rotation damping on/off
    bool isAutoStabilizationEnabled() const; // Check if stabilization is on
    void applyRotationDamping(float deltaTime); // Apply damping to rotation

    // Speed limiter system
    void toggleSpeedLimiter();              // Toggle speed cap on/off
    bool isSpeedLimiterEnabled() const;     // Check if limiter is on
    void applySpeedLimit();                 // Cap velocity if needed
    void setSpeedLimit(float limit);        // Set max speed limit
    float getSpeedLimit() const;            // Get current speed limit

    // Landing system
    LandingState getLandingState() const;
    CelestialBody* getLandedBody() const;
    void checkLandingProximity(const std::vector<CelestialBody*>& bodies);  // Check if can land
    void attemptLanding(CelestialBody* body);  // Try to land on body
    void takeoff();                            // Leave surface
    bool canLandOn(const CelestialBody* body) const;  // Is this body landable?

    // Resource & Damage integration helpers
    // Note: Actual ResourceSystem and DamageSystem are stored in main.cpp
    // These methods provide convenient accessors for ship state
    Vec3 getPosition() const { return physicsBody.position; }
    void setPosition(const Vec3& pos) { physicsBody.position = pos; }
    void setVelocity(const Vec3& vel) { physicsBody.velocity = vel; }
    void setYaw(float y) { yaw = y; updateDirectionVectors(); }
    void setPitch(float p) { pitch = p; updateDirectionVectors(); }
};
