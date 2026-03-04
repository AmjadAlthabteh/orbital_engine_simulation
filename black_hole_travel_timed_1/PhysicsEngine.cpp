#include "PhysicsEngine.hpp"
#include <cmath>

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
    handleCollisions();

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

bool PhysicsEngine::checkCollision(const Body* a, const Body* b) const
{
    Vec3 diff = b->position - a->position;
    float distance = diff.length();
    float minDistance = a->radius + b->radius;
    return distance < minDistance;
}

float PhysicsEngine::predictCollisionTime(const Body* a, const Body* b) const
{
    Vec3 relativePos = b->position - a->position;
    Vec3 relativeVel = b->velocity - a->velocity;
    float minDistance = a->radius + b->radius;

    float a_coeff = relativeVel.dot(relativeVel);
    float b_coeff = 2.0f * relativePos.dot(relativeVel);
    float c_coeff = relativePos.dot(relativePos) - minDistance * minDistance;

    float discriminant = b_coeff * b_coeff - 4.0f * a_coeff * c_coeff;

    if (discriminant < 0.0f || a_coeff == 0.0f)
        return -1.0f;

    float t1 = (-b_coeff - std::sqrt(discriminant)) / (2.0f * a_coeff);
    float t2 = (-b_coeff + std::sqrt(discriminant)) / (2.0f * a_coeff);

    if (t1 > 0.0f) return t1;
    if (t2 > 0.0f) return t2;
    return -1.0f;
}

void PhysicsEngine::handleCollisions()
{
    for (size_t i = 0; i < bodies.size(); ++i)
    {
        for (size_t j = i + 1; j < bodies.size(); ++j)
        {
            if (checkCollision(bodies[i], bodies[j]))
            {
                // Simple elastic collision response
                Vec3 normal = (bodies[j]->position - bodies[i]->position).normalize();
                Vec3 relativeVel = bodies[j]->velocity - bodies[i]->velocity;
                float velAlongNormal = relativeVel.dot(normal);

                if (velAlongNormal > 0) continue;

                float e = 0.8f; // coefficient of restitution
                float j = -(1.0f + e) * velAlongNormal;
                j /= (1.0f / bodies[i]->mass + 1.0f / bodies[j]->mass);

                Vec3 impulse = normal * j;
                bodies[i]->velocity = bodies[i]->velocity - impulse * (1.0f / bodies[i]->mass);
                bodies[j]->velocity = bodies[j]->velocity + impulse * (1.0f / bodies[j]->mass);

                // Separate bodies to prevent overlap
                float overlap = (bodies[i]->radius + bodies[j]->radius) -
                               (bodies[j]->position - bodies[i]->position).length();
                Vec3 separation = normal * (overlap * 0.5f);
                bodies[i]->position = bodies[i]->position - separation;
                bodies[j]->position = bodies[j]->position + separation;
            }
        }
    }
}