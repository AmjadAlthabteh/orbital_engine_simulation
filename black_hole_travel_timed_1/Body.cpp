#include "Body.hpp"

Body::Body(float m, float r)
{
    mass = m;
    inverseMass = mass > 0.0f ? 1.0f / mass : 0.0f;
    radius = r;
    position = Vec3(0, 0, 0);
    velocity = Vec3(0, 0, 0);
    acceleration = Vec3(0, 0, 0);
    forceAccum = Vec3(0, 0, 0);
    isStatic = mass <= 0.0f;
}

void Body::applyForce(const Vec3& force)
{
    if (isStatic) return;

    forceAccum = forceAccum + force;
}

void Body::applyImpulse(const Vec3& impulse)
{
    if (isStatic) return;

    velocity = velocity + impulse * inverseMass;
}

void Body::integrate(float deltaTime)
{
    if (isStatic || inverseMass <= 0.0f)
    {
        clearForces();
        acceleration = Vec3(0, 0, 0);
        return;
    }

    acceleration = forceAccum * inverseMass;

    velocity = velocity + acceleration * deltaTime;
    position = position + velocity * deltaTime;

    clearForces();
}

void Body::clearForces()
{
    forceAccum = Vec3(0, 0, 0);
}

float Body::getSpeed() const
{
    return velocity.length();
}

float Body::kineticEnergy() const
{
    return 0.5f * mass * velocity.lengthSquared();
}
