#include "SolarSystemFactory.hpp"

std::vector<CelestialBody*> SolarSystemFactory::createSimpleSystem()
{
    std::vector<CelestialBody*> system;

    // Create Sun
    CelestialBody* sun = new CelestialBody("Sun", 1000.0f, 2.0f);
    sun->getPhysicsBody().position = Vec3(0, 0, 0);

    // Create Planet
    CelestialBody* planet = new CelestialBody("Planet", 1.0f, 0.5f);
    planet->getPhysicsBody().position = Vec3(10, 0, 0);
    planet->getPhysicsBody().velocity = Vec3(0, 3.0f, 0);

    system.push_back(sun);
    system.push_back(planet);

    return system;
}