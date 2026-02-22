#include "Body.hpp"

Body::Body(float m)
{
    mass = m;
    position = Vec3(0, 0, 0);
    velocity = Vec3(0, 0, 0);
    forceAccum = Vec3(0, 0, 0);
}

void Body::applyForce(const Vec3& force)
{
    forceAccum = forceAccum + force;
}

void Body::integrate(float deltaTime)
{
    if (mass <= 0.0f) return;

    Vec3 acceleration = forceAccum * (1.0f / mass);

    velocity = velocity + acceleration * deltaTime;
    position = position + velocity * deltaTime;

    clearForces();
}

void Body::clearForces()
{
    forceAccum = Vec3(0, 0, 0);
}