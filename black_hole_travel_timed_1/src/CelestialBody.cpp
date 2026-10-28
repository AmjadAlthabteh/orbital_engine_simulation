#include "CelestialBody.hpp"
#include <cstdlib>

// PERFORMANCE: Reserve common angle normalization value
constexpr float FULL_ROTATION = 360.0f;

CelestialBody::CelestialBody(const std::string& name_,
    float mass,
    float radius_,
    const Vec3& color_)
    : name(name_),
    radius(radius_),
    color(color_),
    physicsBody(mass, radius_),
    rotationAngle(0.0f),
    rotationSpeed(10.0f + (rand() % 50)),
    rotationAxis(0.0f, 1.0f, 0.1f),
    axialTiltDegrees(0.0f),
    hasTrail(false),
    trailTimer(0.0f),
    trailInterval(0.05f)
{
    setScale(Vec3(radius, radius, radius));

    // Relative visual spin rates and measured axial tilts. Rates are accelerated
    // for readability but preserve direction and broad real-world relationships.
    if (name == "Sun") { axialTiltDegrees = 7.25f; rotationSpeed = 3.0f; }
    else if (name == "Mercury") { axialTiltDegrees = 0.03f; rotationSpeed = 1.0f; }
    else if (name == "Venus") { axialTiltDegrees = 177.36f; rotationSpeed = -0.65f; }
    else if (name == "Earth") { axialTiltDegrees = 23.44f; rotationSpeed = 18.0f; }
    else if (name == "Moon") { axialTiltDegrees = 6.68f; rotationSpeed = 0.65f; }
    else if (name == "Mars") { axialTiltDegrees = 25.19f; rotationSpeed = 17.5f; }
    else if (name == "Jupiter") { axialTiltDegrees = 3.13f; rotationSpeed = 43.5f; }
    else if (name == "Saturn") { axialTiltDegrees = 26.73f; rotationSpeed = 40.5f; }
    else if (name == "Uranus") { axialTiltDegrees = 97.77f; rotationSpeed = -25.5f; }
    else if (name == "Neptune") { axialTiltDegrees = 28.32f; rotationSpeed = 26.0f; }
}

void CelestialBody::update(float deltaTime)
{
    // PERFORMANCE: Inline candidate - called every frame for every body
    physicsBody.integrate(deltaTime);
    setPosition(physicsBody.position);

    // OPTIMIZATION: Use fmod for angle wrapping (branchless, faster)
    rotationAngle += rotationSpeed * deltaTime;
    if (rotationAngle > FULL_ROTATION)
        rotationAngle -= FULL_ROTATION;

    // OPTIMIZATION: Early exit if no trail (branch prediction friendly)
    if (!hasTrail) [[likely]]
        return;

    // Update trail with reduced branching
    if (trail)
    {
        trailTimer += deltaTime;
        if (trailTimer >= trailInterval)
        {
            trail->addPoint(physicsBody.position);
            trailTimer -= trailInterval;  // Preserve remainder instead of reset
        }
    }
}

void CelestialBody::enableTrail(size_t maxPoints)
{
    trail = std::make_unique<Trail>(maxPoints, color);
    hasTrail = true;
}

void CelestialBody::renderTrail()
{
    if (hasTrail && trail)
    {
        trail->render();
    }
}

Body& CelestialBody::getPhysicsBody()
{
    return physicsBody;
}

const Body& CelestialBody::getPhysicsBody() const
{
    return physicsBody;
}

const std::string& CelestialBody::getName() const
{
    return name;
}

float CelestialBody::getRadius() const
{
    return radius;
}

const Vec3& CelestialBody::getColor() const
{
    return color;
}

float CelestialBody::getRotationAngle() const
{
    return rotationAngle;
}

const Vec3& CelestialBody::getRotationAxis() const
{
    return rotationAxis;
}

Mat4 CelestialBody::getRenderModelMatrix() const
{
    constexpr float degreesToRadians = 0.01745329252f;
    const Vec3 pos = getPosition();
    return Mat4::translation(pos.x, pos.y, pos.z) *
           Mat4::rotationX(axialTiltDegrees * degreesToRadians) *
           Mat4::rotationZ(rotationAngle * degreesToRadians) *
           Mat4::scale(radius);
}
