#include "CelestialBody.hpp"

CelestialBody::CelestialBody(const std::string& name_,
    float mass,
    float radius_)
    : name(name_),
    radius(radius_),
    physicsBody(mass)
{
    setScale(Vec3(radius, radius, radius));
}

void CelestialBody::update(float deltaTime)
{
    physicsBody.integrate(deltaTime);

    // Sync physics position with entity transform
    setPosition(physicsBody.position);
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