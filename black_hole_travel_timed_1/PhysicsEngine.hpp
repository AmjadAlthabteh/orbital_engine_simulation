#pragma once
#include <vector>
#include "Body.hpp"

// Handles gravity + integration.
class PhysicsEngine
{
private:
    float G;

    float length(const sf::Vector2f& v) const;

public:
    PhysicsEngine(float gravitationalConstant);

    void update(std::vector<Body>& bodies, float dt);
};