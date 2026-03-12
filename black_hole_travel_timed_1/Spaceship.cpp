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
      rotationSpeed(90.0f),         // 90 degrees per second
      forward(0.0f, 0.0f, 1.0f),    // Initial forward direction
      up(0.0f, 1.0f, 0.0f),         // Initial up direction
      right(1.0f, 0.0f, 0.0f),      // Initial right direction
      hasExhaustTrail(false),
      exhaustTimer(0.0f),
      exhaustInterval(0.02f),       // Faster trail updates than planets
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
    // If landed, stay locked to planet surface
    if (landingState == LandingState::LANDED && landedOn != nullptr)
    {
        // Lock position to planet surface (at landing offset)
        Vec3 planetPos = landedOn->getPhysicsBody().position;
        Vec3 toPlanet = (planetPos - physicsBody.position).normalize();
        float surfaceDistance = landedOn->getRadius() + radius + 0.1f;  // Slight hover above surface

        physicsBody.position = planetPos - toPlanet * surfaceDistance;
        physicsBody.velocity = Vec3(0, 0, 0);  // Zero velocity when landed
        physicsBody.acceleration = Vec3(0, 0, 0);
    }
    else
    {
        // NOTE: Physics integration is handled by PhysicsEngine, not here
        // We just sync our position with the physics body
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
    Vec3 thrustForce = forward * thrustPower;
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

    // Zero out velocity
    physicsBody.velocity = Vec3(0, 0, 0);
    physicsBody.acceleration = Vec3(0, 0, 0);

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
