#pragma once

#include "CelestialBody.hpp"
#include <vector>

class SolarSystemFactory
{
public:
    static std::vector<CelestialBody*> createSimpleSystem();
};