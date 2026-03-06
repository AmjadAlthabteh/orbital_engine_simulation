#include "PhysicsEngine.hpp"
#include <cmath>
#include <iostream>

PhysicsEngine::PhysicsEngine(float G)
{
    gravitationalConstant = G;
    recentCollisions.reserve(100);  // Pre-allocate for performance
}

void PhysicsEngine::addBody(Body* body)
{
    bodies.push_back(body);
}

void PhysicsEngine::update(float deltaTime)
{
    applyGravity();
    handleCollisions();
    updateCollisionMarkers(deltaTime);

    // Integrate all bodies (with null pointer safety check)
    for (auto* body : bodies)
    {
        if (body != nullptr)
        {
            body->integrate(deltaTime);
        }
    }
}

const std::vector<CollisionEvent>& PhysicsEngine::getRecentCollisions() const
{
    return recentCollisions;
}

void PhysicsEngine::updateCollisionMarkers(float deltaTime)
{
    // Update timestamps and remove old collision markers (older than 3 seconds)
    for (size_t i = 0; i < recentCollisions.size(); )
    {
        recentCollisions[i].timestamp += deltaTime;

        if (recentCollisions[i].timestamp > 3.0f)  // Fade after 3 seconds
        {
            // Remove this collision marker
            recentCollisions.erase(recentCollisions.begin() + i);
        }
        else
        {
            ++i;
        }
    }
}

void PhysicsEngine::applyGravity()
{
    // SAFETY: Check bodies vector is valid
    if (bodies.empty() || bodies.size() == 0)
        return;

    const size_t bodyCount = bodies.size();

    for (size_t i = 0; i < bodyCount; ++i)
    {
        // SAFETY: Verify index is in range
        if (i >= bodies.size()) break;
        if (bodies[i] == nullptr) continue;

        for (size_t j = i + 1; j < bodyCount; ++j)
        {
            // SAFETY: Verify index is in range
            if (j >= bodies.size()) break;
            if (bodies[j] == nullptr) continue;

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
    // SAFETY: Check bodies vector is valid
    if (bodies.empty() || bodies.size() < 2)
        return;

    const size_t bodyCount = bodies.size();

    for (size_t i = 0; i < bodyCount; ++i)
    {
        // SAFETY: Verify index and pointer
        if (i >= bodies.size()) break;
        if (bodies[i] == nullptr) continue;

        for (size_t j = i + 1; j < bodyCount; ++j)
        {
            // SAFETY: Verify index and pointer
            if (j >= bodies.size()) break;
            if (bodies[j] == nullptr) continue;

            if (checkCollision(bodies[i], bodies[j]))
            {
                // Calculate collision point (midpoint between surfaces)
                Vec3 normal = (bodies[j]->position - bodies[i]->position).normalize();
                Vec3 collisionPoint = bodies[i]->position + normal * bodies[i]->radius;

                // Calculate relative velocity
                Vec3 relativeVel = bodies[j]->velocity - bodies[i]->velocity;
                float velAlongNormal = relativeVel.dot(normal);

                if (velAlongNormal > 0) continue;

                // Calculate collision intensity for visualization
                float collisionSpeed = relativeVel.length();

                // Simple elastic collision response
                float e = 0.8f; // coefficient of restitution
                float j_impulse = -(1.0f + e) * velAlongNormal;
                j_impulse /= (1.0f / bodies[i]->mass + 1.0f / bodies[j]->mass);

                Vec3 impulse = normal * j_impulse;
                bodies[i]->velocity = bodies[i]->velocity - impulse * (1.0f / bodies[i]->mass);
                bodies[j]->velocity = bodies[j]->velocity + impulse * (1.0f / bodies[j]->mass);

                // Separate bodies to prevent overlap
                float overlap = (bodies[i]->radius + bodies[j]->radius) -
                               (bodies[j]->position - bodies[i]->position).length();
                Vec3 separation = normal * (overlap * 0.5f);
                bodies[i]->position = bodies[i]->position - separation;
                bodies[j]->position = bodies[j]->position + separation;

                // Record collision event for visualization
                CollisionEvent event;
                event.position = collisionPoint;
                event.timestamp = 0.0f;  // Will be updated by timer
                event.intensity = collisionSpeed;
                event.bodyA = bodies[i];
                event.bodyB = bodies[j];
                recentCollisions.push_back(event);

                // Console output for debugging
                std::cout << "*** COLLISION DETECTED ***\n";
                std::cout << "  Position: (" << collisionPoint.x << ", " << collisionPoint.y << ", " << collisionPoint.z << ")\n";
                std::cout << "  Impact speed: " << collisionSpeed << " m/s\n";
                std::cout << "  Bodies: Mass " << bodies[i]->mass << " vs Mass " << bodies[j]->mass << "\n";
            }
        }
    }
}