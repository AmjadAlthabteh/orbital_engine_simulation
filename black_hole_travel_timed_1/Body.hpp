// mass velocity position, accelerations , radius and trail history
#pragma once
#include <SFML/Graphics.hpp>
#include <deque>

// Represents a physical body in space.
// This stores only state and simple helpers.
// No gravity math here.
class Body
{
public:
    float mass;           // Mass of object
    float radius;         // Render radius
    bool isBlackHole;     // Special rendering flag

    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;

    sf::Color color;

    // Trail history for motion blur effect
    std::deque<sf::Vector2f> trail;

    Body(float m,
        float r,
        const sf::Vector2f& pos,
        const sf::Vector2f& vel,
        sf::Color c,
        bool blackHole = false);

    void resetAcceleration();
    void updateTrail();
};