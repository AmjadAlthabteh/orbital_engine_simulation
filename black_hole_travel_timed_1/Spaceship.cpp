#include "Spaceship.hpp"
#include "CelestialBody.hpp"
#include <cmath>
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Spaceship::Spaceship(float mass, float radius_, const Vec3& color_)
    : physicsBody(mass, radius_),
      radius(radius_),
      color(color_),
      thrustPower(50.0f),           // Default thrust force
      isThrusting(false),
      yaw(0.0f),                    // Start facing forward (+Z)
      pitch(0.0f),                  // Level orientation
      roll(0.0f),                   // No initial roll
      rotationSpeed(120.0f),        // 120 degrees per second - increased for better maneuverability
      forward(0.0f, 0.0f, 1.0f),    // Initial forward direction
      up(0.0f, 1.0f, 0.0f),         // Initial up direction
      right(1.0f, 0.0f, 0.0f),      // Initial right direction
      isBarrelRolling(false),       // Not barrel rolling yet!
      barrelRollProgress(0.0f),
      barrelRollSpeed(720.0f),      // 2 full rotations per second - CRAZY FAST!
      autoStabilizationEnabled(false), // Auto-stabilization off by default
      angularVelocityYaw(0.0f),
      angularVelocityPitch(0.0f),
      rotationDamping(0.95f),       // 95% damping per second
      speedLimiterEnabled(false),   // Speed limiter off by default
      maxSpeedLimit(100.0f),        // Default max speed: 100 units/s
      isTurboBoostActive(false),    // Boost not active
      turboBoostMultiplier(3.0f),   // 3x thrust during boost!
      turboBoostDuration(2.0f),     // 2 seconds of pure power
      turboBoostTimer(0.0f),
      turboBoostCooldown(5.0f),     // 5 second cooldown
      turboBoostCooldownTimer(0.0f),
      hasExhaustTrail(false),
      exhaustTimer(0.0f),
      exhaustInterval(0.02f),       // Faster trail updates than planets
      rainbowExhaustMode(false),    // Start with normal exhaust
      exhaustColorPhase(0.0f),      // Color animation starts at 0
      landingState(LandingState::FLYING),
      landedOn(nullptr),
      landingOffset(0.0f, 0.0f, 0.0f),
      landingDistThreshold(3.0f),   // Distance multiplier for landing
      landingVelThreshold(5.0f)     // Max velocity to land
{
    setScale(Vec3(radius, radius, radius));
    updateDirectionVectors();
}

void Spaceship::update(float deltaTime)
{
    // Update CRAZY barrel roll animation!
    if (isBarrelRolling)
    {
        updateBarrelRoll(deltaTime);
    }

    // Update TURBO BOOST timer!
    updateTurboBoost(deltaTime);

    // Update rainbow exhaust color phase
    if (rainbowExhaustMode)
    {
        exhaustColorPhase += deltaTime * 2.0f;  // Cycle through colors
        if (exhaustColorPhase > 6.28318f) exhaustColorPhase = 0.0f;  // Wrap at 2*PI
    }

    // If landed, stay locked to planet surface
    if (landingState == LandingState::LANDED && landedOn != nullptr)
    {
        // Lock position to planet surface (at landing offset)
        Vec3 planetPos = landedOn->getPhysicsBody().position;
        Vec3 toPlanet = (planetPos - physicsBody.position).normalize();
        float surfaceDistance = landedOn->getRadius() + radius + 0.1f;  // Slight hover above surface

        physicsBody.position = planetPos - toPlanet * surfaceDistance;
        physicsBody.velocity = Vec3(0, 0, 0);  // Zero velocity when landed
        physicsBody.clearForces();  // Clear all forces when landed
    }
    else
    {
        // NOTE: Physics integration is handled by PhysicsEngine, not here
        // We just sync our position with the physics body

        // Apply auto-stabilization if enabled
        if (autoStabilizationEnabled)
        {
            applyRotationDamping(deltaTime);
        }

        // Apply speed limiter if enabled
        if (speedLimiterEnabled)
        {
            applySpeedLimit();
        }
    }

    setPosition(physicsBody.position);

    // Update exhaust trail when thrusting
    if (hasExhaustTrail && exhaustTrail && isThrusting)
    {
        exhaustTimer += deltaTime;
        if (exhaustTimer >= exhaustInterval)
        {
            exhaustTrail->addPoint(physicsBody.position);
            exhaustTimer = 0.0f;
        }
    }
}

void Spaceship::applyThrust(float deltaTime)
{
    // Apply thrust force in the forward direction
    float effectiveThrust = thrustPower;

    // TURBO BOOST MULTIPLIER!
    if (isTurboBoostActive)
    {
        effectiveThrust *= turboBoostMultiplier;
    }

    Vec3 thrustForce = forward * effectiveThrust;
    physicsBody.applyForce(thrustForce);
    isThrusting = true;
}

void Spaceship::applyReverseThrust(float deltaTime)
{
    // Apply thrust force in the reverse direction
    Vec3 thrustForce = forward * (-thrustPower * 0.5f);  // Reverse is half power
    physicsBody.applyForce(thrustForce);
    isThrusting = true;
}

void Spaceship::stopThrust()
{
    isThrusting = false;
}

void Spaceship::rotate(float yawDelta, float pitchDelta)
{
    // Track angular velocities for auto-stabilization
    angularVelocityYaw = yawDelta;
    angularVelocityPitch = pitchDelta;

    // Update yaw and pitch angles
    yaw += yawDelta;
    pitch += pitchDelta;

    // Clamp pitch to avoid gimbal lock (same as camera)
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Normalize yaw to [0, 360]
    while (yaw >= 360.0f) yaw -= 360.0f;
    while (yaw < 0.0f) yaw += 360.0f;

    // Update direction vectors
    updateDirectionVectors();
}

void Spaceship::updateDirectionVectors()
{
    // Convert degrees to radians
    float yawRad = yaw * (M_PI / 180.0f);
    float pitchRad = pitch * (M_PI / 180.0f);

    // Calculate forward vector from yaw and pitch (spherical coordinates)
    forward.x = std::sin(yawRad) * std::cos(pitchRad);
    forward.y = std::sin(pitchRad);
    forward.z = std::cos(yawRad) * std::cos(pitchRad);
    forward = forward.normalize();

    // Calculate right vector (perpendicular to forward and world up)
    Vec3 worldUp(0.0f, 1.0f, 0.0f);
    right = forward.cross(worldUp).normalize();

    // Calculate up vector (perpendicular to forward and right)
    up = right.cross(forward).normalize();
}

Body& Spaceship::getPhysicsBody()
{
    return physicsBody;
}

const Body& Spaceship::getPhysicsBody() const
{
    return physicsBody;
}

float Spaceship::getSpeed() const
{
    return physicsBody.velocity.length();
}

Vec3 Spaceship::getVelocity() const
{
    return physicsBody.velocity;
}

Vec3 Spaceship::getForward() const
{
    return forward;
}

Vec3 Spaceship::getUp() const
{
    return up;
}

Vec3 Spaceship::getRight() const
{
    return right;
}

float Spaceship::getYaw() const
{
    return yaw;
}

float Spaceship::getPitch() const
{
    return pitch;
}

bool Spaceship::getIsThrusting() const
{
    return isThrusting;
}

float Spaceship::getRotationSpeed() const
{
    return rotationSpeed;
}

const Vec3& Spaceship::getColor() const
{
    return color;
}

float Spaceship::getRadius() const
{
    return radius;
}

Mat4 Spaceship::getOrientedModelMatrix() const
{
    // Create rotation matrix from forward/up/right vectors
    // This orients the ship to point in the direction of travel

    // Translation to ship position (takes 3 floats, not Vec3)
    Mat4 translation = Mat4::translation(physicsBody.position.x, physicsBody.position.y, physicsBody.position.z);

    // Scale matrix (takes single float for uniform scale)
    Mat4 scaleMatrix = Mat4::scale(radius);

    // Rotation matrix from orientation vectors
    // Build a rotation matrix where:
    // - Column 0: right vector
    // - Column 1: up vector
    // - Column 2: forward vector
    Mat4 rotation = Mat4::identity();
    rotation.m[0] = right.x;    rotation.m[4] = up.x;    rotation.m[8] = forward.x;
    rotation.m[1] = right.y;    rotation.m[5] = up.y;    rotation.m[9] = forward.y;
    rotation.m[2] = right.z;    rotation.m[6] = up.z;    rotation.m[10] = forward.z;

    // Combine: T * R * S (translation, rotation, scale)
    return translation * rotation * scaleMatrix;
}

void Spaceship::enableExhaustTrail(size_t maxPoints)
{
    // Orange/red exhaust color
    Vec3 exhaustColor(1.0f, 0.5f, 0.1f);
    exhaustTrail = std::make_unique<Trail>(maxPoints, exhaustColor);
    hasExhaustTrail = true;
}

void Spaceship::renderExhaustTrail()
{
    if (hasExhaustTrail && exhaustTrail)
    {
        exhaustTrail->render();
    }
}

void Spaceship::setThrustPower(float power)
{
    thrustPower = power;
}

void Spaceship::setRotationSpeed(float speed)
{
    rotationSpeed = speed;
}

// Landing system implementation
LandingState Spaceship::getLandingState() const
{
    return landingState;
}

CelestialBody* Spaceship::getLandedBody() const
{
    return landedOn;
}

bool Spaceship::canLandOn(const CelestialBody* body) const
{
    // Can only land on rocky planets with surfaces
    const std::string& name = body->getName();

    // Landable: Earth, Moon, Mars, Mercury (not gas giants, not Sun, not Black Hole)
    return (name == "Earth" || name == "Moon" || name == "Mars" || name == "Mercury");
}

void Spaceship::checkLandingProximity(const std::vector<CelestialBody*>& bodies)
{
    // If already landed, don't check
    if (landingState == LandingState::LANDED)
        return;

    float currentSpeed = getSpeed();
    bool foundApproaching = false;

    // Check all landable bodies
    for (CelestialBody* body : bodies)
    {
        if (!canLandOn(body))
            continue;

        Vec3 toBody = body->getPhysicsBody().position - physicsBody.position;
        float distance = toBody.length();
        float surfaceDistance = distance - body->getRadius() - radius;

        // Check if within landing range
        float landingRange = landingDistThreshold * (body->getRadius() + radius);

        if (surfaceDistance < landingRange && currentSpeed < landingVelThreshold)
        {
            landingState = LandingState::APPROACHING;
            foundApproaching = true;
            break;  // Only approach one body at a time
        }
    }

    // If not approaching any body, set to flying
    if (!foundApproaching && landingState == LandingState::APPROACHING)
    {
        landingState = LandingState::FLYING;
    }
}

void Spaceship::attemptLanding(CelestialBody* body)
{
    if (landingState != LandingState::APPROACHING)
    {
        std::cout << "Cannot land - not in approach range or moving too fast!\n";
        return;
    }

    if (!canLandOn(body))
    {
        std::cout << "Cannot land on " << body->getName() << " - no solid surface!\n";
        return;
    }

    // Execute landing
    landingState = LandingState::LANDED;
    landedOn = body;

    // Calculate landing offset (position relative to planet center)
    Vec3 toPlanet = (body->getPhysicsBody().position - physicsBody.position).normalize();
    landingOffset = toPlanet * (body->getRadius() + radius + 0.1f);

    // Zero out velocity and forces
    physicsBody.velocity = Vec3(0, 0, 0);
    physicsBody.clearForces();

    std::cout << "Successfully landed on " << body->getName() << "!\n";
    std::cout << "Press T to take off.\n";
}

void Spaceship::takeoff()
{
    if (landingState != LandingState::LANDED)
    {
        std::cout << "Cannot take off - not currently landed!\n";
        return;
    }

    // Apply upward impulse for takeoff
    if (landedOn != nullptr)
    {
        Vec3 planetPos = landedOn->getPhysicsBody().position;
        Vec3 awayFromPlanet = (physicsBody.position - planetPos).normalize();
        physicsBody.velocity = awayFromPlanet * 3.0f;  // Launch velocity

        std::cout << "Taking off from " << landedOn->getName() << "!\n";
    }

    // Reset landing state
    landingState = LandingState::FLYING;
    landedOn = nullptr;
    landingOffset = Vec3(0, 0, 0);
}

// Auto-stabilization system implementation
void Spaceship::toggleAutoStabilization()
{
    autoStabilizationEnabled = !autoStabilizationEnabled;
    if (autoStabilizationEnabled)
    {
        std::cout << "Auto-stabilization ENABLED - Ship will auto-dampen rotation\n";
    }
    else
    {
        std::cout << "Auto-stabilization DISABLED - Manual control only\n";
    }
}

bool Spaceship::isAutoStabilizationEnabled() const
{
    return autoStabilizationEnabled;
}

void Spaceship::applyRotationDamping(float deltaTime)
{
    // Apply exponential damping to angular velocities
    float dampingFactor = std::pow(rotationDamping, deltaTime);
    angularVelocityYaw *= dampingFactor;
    angularVelocityPitch *= dampingFactor;

    // Apply residual rotation (decaying over time)
    yaw += angularVelocityYaw;
    pitch += angularVelocityPitch;

    // Clamp pitch to avoid gimbal lock
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Normalize yaw to [0, 360]
    while (yaw >= 360.0f) yaw -= 360.0f;
    while (yaw < 0.0f) yaw += 360.0f;

    // Update direction vectors
    updateDirectionVectors();
}

// Speed limiter system implementation
void Spaceship::toggleSpeedLimiter()
{
    speedLimiterEnabled = !speedLimiterEnabled;
    if (speedLimiterEnabled)
    {
        std::cout << "Speed limiter ENABLED - Max speed: " << maxSpeedLimit << " units/s\n";
    }
    else
    {
        std::cout << "Speed limiter DISABLED - No speed cap\n";
    }
}

bool Spaceship::isSpeedLimiterEnabled() const
{
    return speedLimiterEnabled;
}

void Spaceship::applySpeedLimit()
{
    float currentSpeed = getSpeed();
    if (currentSpeed > maxSpeedLimit)
    {
        // Scale velocity down to max limit
        Vec3 velocityDirection = physicsBody.velocity.normalize();
        physicsBody.velocity = velocityDirection * maxSpeedLimit;
    }
}

void Spaceship::setSpeedLimit(float limit)
{
    maxSpeedLimit = limit;
    std::cout << "Speed limit set to: " << maxSpeedLimit << " units/s\n";
}

float Spaceship::getSpeedLimit() const
{
    return maxSpeedLimit;
}

// ============================================
// CRAZY NEW FEATURES IMPLEMENTATION!
// ============================================

void Spaceship::initiateBarrelRoll()
{
    if (!isBarrelRolling)
    {
        isBarrelRolling = true;
        barrelRollProgress = 0.0f;
        std::cout << "DO A BARREL ROLL!!!\n";
    }
}

void Spaceship::updateBarrelRoll(float deltaTime)
{
    if (isBarrelRolling)
    {
        barrelRollProgress += barrelRollSpeed * deltaTime;

        if (barrelRollProgress >= 360.0f)
        {
            // Complete the barrel roll!
            barrelRollProgress = 0.0f;
            isBarrelRolling = false;
            roll = 0.0f;
            std::cout << "Barrel roll complete! Nice moves!\n";
        }
        else
        {
            roll = barrelRollProgress;
        }

        // Update direction vectors to account for roll
        updateDirectionVectors();
    }
}

void Spaceship::activateTurboBoost()
{
    if (!isTurboBoostActive && turboBoostCooldownTimer <= 0.0f)
    {
        isTurboBoostActive = true;
        turboBoostTimer = turboBoostDuration;
        std::cout << "TURBO BOOST ACTIVATED! GOTTA GO FAST!!!\n";
    }
    else if (turboBoostCooldownTimer > 0.0f)
    {
        std::cout << "Turbo boost on cooldown... " << turboBoostCooldownTimer << "s remaining\n";
    }
}

void Spaceship::updateTurboBoost(float deltaTime)
{
    if (isTurboBoostActive)
    {
        turboBoostTimer -= deltaTime;
        if (turboBoostTimer <= 0.0f)
        {
            isTurboBoostActive = false;
            turboBoostCooldownTimer = turboBoostCooldown;
            std::cout << "Turbo boost depleted! Starting cooldown...\n";
        }
    }
    else if (turboBoostCooldownTimer > 0.0f)
    {
        turboBoostCooldownTimer -= deltaTime;
        if (turboBoostCooldownTimer < 0.0f)
        {
            turboBoostCooldownTimer = 0.0f;
        }
    }
}

void Spaceship::toggleRainbowExhaust()
{
    rainbowExhaustMode = !rainbowExhaustMode;
    if (rainbowExhaustMode)
    {
        std::cout << "RAINBOW EXHAUST MODE ACTIVATED! Taste the rainbow!\n";
    }
    else
    {
        std::cout << "Rainbow exhaust mode disabled. Back to normal.\n";
    }
}

Vec3 Spaceship::getRainbowColor()
{
    if (!rainbowExhaustMode)
    {
        return Vec3(1.0f, 0.5f, 0.1f);  // Normal orange exhaust
    }

    // Generate rainbow color using HSV -> RGB conversion
    // Hue cycles from 0 to 360 degrees (0 to 2*PI radians)
    float hue = exhaustColorPhase;
    float saturation = 1.0f;
    float value = 1.0f;

    // Simple HSV to RGB conversion
    float c = value * saturation;
    float x = c * (1.0f - std::abs(std::fmod(hue / 1.047197f, 2.0f) - 1.0f));  // 1.047197 = PI/3
    float m = value - c;

    Vec3 rgb;
    if (hue < 1.047197f)
        rgb = Vec3(c, x, 0);
    else if (hue < 2.094395f)
        rgb = Vec3(x, c, 0);
    else if (hue < 3.141593f)
        rgb = Vec3(0, c, x);
    else if (hue < 4.188790f)
        rgb = Vec3(0, x, c);
    else if (hue < 5.235988f)
        rgb = Vec3(x, 0, c);
    else
        rgb = Vec3(c, 0, x);

    return Vec3(rgb.x + m, rgb.y + m, rgb.z + m);
}
