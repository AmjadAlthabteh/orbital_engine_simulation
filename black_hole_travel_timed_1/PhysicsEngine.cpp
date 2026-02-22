#include "PhysicsEngine.hpp"

PhysicsEngine::PhysicsEngine(float G)
{
    gravitationalConstant = G;
}

void PhysicsEngine::addBody(Body* body)
{
    bodies.push_back(body);
}

void PhysicsEngine::update(float deltaTime)
{
    applyGravity();

    for (auto* body : bodies)
    {
        body->integrate(deltaTime);
    }
}

void PhysicsEngine::applyGravity()
{
    for (size_t i = 0; i < bodies.size(); ++i)
    {
        for (size_t j = i + 1; j < bodies.size(); ++j)
        {
            Vec3 direction = bodies[j]->position - bodies[i]->position;
            float distance = direction.length();

            if (distance < 0.01f) continue;

            float forceMagnitude =
                gravitationalConstant *
                (bodies[i]->mass * bodies[j]->mass) /
                (distance * distance);

            Vec3 force = direction.normalize() * forceMagnitude;

            bodies[i]->applyForce(force);
            bodies[j]->applyForce(force * -1.0f);
        }
    }
}