#include "CelestialBody.hpp"

CelestialBody::CelestialBody(const std::string& name_,
    float mass,
    float radius_,
    const Vec3& color_)
    : name(name_),
    radius(radius_),
    color(color_),
    physicsBody(mass, radius_)
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

const Vec3& CelestialBody::getColor() const
{
    return color;
}