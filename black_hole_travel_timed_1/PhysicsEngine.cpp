#include "PhysicsEngine.hpp"

PhysicsEngine::PhysicsEngine(float gravitationalConstant)
    : G(gravitationalConstant)
{
}

void PhysicsEngine::update(std::vector<CelestialBody>& bodies, float dt)
{
    for (auto& body : bodies)
        body.resetAcceleration();

    for (size_t i = 0; i < bodies.size(); ++i)
    {
        for (size_t j = i + 1; j < bodies.size(); ++j)
        {
            Vec3 direction = bodies[j].position - bodies[i].position;
            float distanceSq = direction.lengthSquared();

            if (distanceSq < 0.01f) continue;

            float force = G * bodies[i].mass * bodies[j].mass / distanceSq;
            Vec3 forceDir = direction.normalize();

            Vec3 accel_i = forceDir * (force / bodies[i].mass);
            Vec3 accel_j = forceDir * (-force / bodies[j].mass);

            bodies[i].acceleration += accel_i;
            bodies[j].acceleration += accel_j;
        }
    }

    for (auto& body : bodies)
    {
        body.velocity += body.acceleration * dt;
        body.position += body.velocity * dt;
    }
}