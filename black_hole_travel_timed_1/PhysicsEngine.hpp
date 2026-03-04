#pragma once
#include "Body.hpp"
#include <vector>

class PhysicsEngine
{
private:
    std::vector<Body*> bodies;
    float gravitationalConstant;

public:
    PhysicsEngine(float G = 0.1f);

    void addBody(Body* body);
    void update(float deltaTime);
    bool checkCollision(const Body* a, const Body* b) const;
    float predictCollisionTime(const Body* a, const Body* b) const;

private:
    void applyGravity();
    void handleCollisions();
};