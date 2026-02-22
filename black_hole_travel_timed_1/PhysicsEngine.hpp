#pragma once
#include <vector>
#include "CelestialBody.hpp"

class PhysicsEngine
{
public:
    float G;

    PhysicsEngine(float gravitationalConstant);

    void update(std::vector<CelestialBody>& bodies, float dt);
};