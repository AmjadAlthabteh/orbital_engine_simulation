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

private:
    void applyGravity();
};