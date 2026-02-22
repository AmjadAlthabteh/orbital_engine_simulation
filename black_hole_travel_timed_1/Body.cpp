#include "Body.hpp"

Body::Body(float m,
    float r,
    const sf::Vector2f& pos,
    const sf::Vector2f& vel,
    sf::Color c,
    bool blackHole)
    : mass(m),
    radius(r),
    isBlackHole(blackHole),
    position(pos),
    velocity(vel),
    acceleration(0.f, 0.f),
    color(c)
{
}

// Clears acceleration each frame.
void Body::resetAcceleration()
{
    acceleration = { 0.f, 0.f };
}

// Stores previous positions for trail effect.
void Body::updateTrail()
{
    trail.push_back(position);

    // Keep trail size limited.
    if (trail.size() > 250)
        trail.pop_front();
}