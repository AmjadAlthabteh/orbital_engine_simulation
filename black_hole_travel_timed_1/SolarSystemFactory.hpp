#pragma once

#include "CelestialBody.hpp"
#include <vector>

class SolarSystemFactory
{
public:
    static std::vector<CelestialBody*> createSimpleSystem();
};

// solar system math and adjust the math 