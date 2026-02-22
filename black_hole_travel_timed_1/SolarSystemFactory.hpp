#pragma once
#include <vector>
#include "CelestialBody.hpp"

class SolarSystemFactory
{
public:
    static void createSolarSystem(std::vector<CelestialBody>& bodies);
};