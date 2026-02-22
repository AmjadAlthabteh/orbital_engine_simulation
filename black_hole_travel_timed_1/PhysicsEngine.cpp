#include "PhysicsEngine.hpp"
#include <cmath>

// Vector length.
float PhysicsEngine::length(const sf::Vector2f& v) const
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

PhysicsEngine::PhysicsEngine(float gravitationalConstant)
    : G(gravitationalConstant)
{
}

// Main N-body solver.
void PhysicsEngine::update(std::vector<Body>& bodies, float dt)
{
    // Reset accelerations.
    for (auto& body : bodies)
        body.resetAcceleration();

    // Compute pairwise gravity.
    for (size_t i = 0; i < bodies.size(); ++i)
    {
        for (size_t j = 0; j < bodies.size(); ++j)
        {
            if (i == j) continue;

            sf::Vector2f direction =
                bodies[j].position - bodies[i].position;

            float distance = length(direction);

            if (distance < 5.f) continue;

            direction /= distance;

            float force =
                (G * bodies[j].mass) /
                (distance * distance);

            bodies[i].acceleration += direction * force;
        }
    }

    // Semi-implicit Euler integration.
    for (auto& body : bodies)
    {
        body.velocity += body.acceleration * dt;
        body.position += body.velocity * dt;

        body.updateTrail();
    }
}