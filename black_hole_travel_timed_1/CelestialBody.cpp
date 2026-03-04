#include "CelestialBody.hpp"
#include <cstdlib>

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
    hasTrail(false),
    trailTimer(0.0f),
    trailInterval(0.05f)
{
    setScale(Vec3(radius, radius, radius));
}

void CelestialBody::update(float deltaTime)
{
    physicsBody.integrate(deltaTime);
    setPosition(physicsBody.position);

    rotationAngle += rotationSpeed * deltaTime;
    if (rotationAngle > 360.0f)
        rotationAngle -= 360.0f;

    if (hasTrail && trail)
    {
        trailTimer += deltaTime;
        if (trailTimer >= trailInterval)
        {
            trail->addPoint(physicsBody.position);
            trailTimer = 0.0f;
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