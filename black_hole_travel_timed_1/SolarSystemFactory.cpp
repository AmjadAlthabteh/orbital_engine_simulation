#include "SolarSystemFactory.hpp"

std::vector<CelestialBody*> SolarSystemFactory::createSimpleSystem()
{
    std::vector<CelestialBody*> system;

    // Create Sun at center
    CelestialBody* sun = new CelestialBody("Sun", 1000.0f, 2.0f);
    sun->getPhysicsBody().position = Vec3(0, 0, 0);

    // Inner Planet (Mercury-like)
    CelestialBody* mercury = new CelestialBody("Mercury", 0.5f, 0.3f);
    mercury->getPhysicsBody().position = Vec3(6, 0, 0);
    mercury->getPhysicsBody().velocity = Vec3(0, 4.0f, 0);

    // Earth-like Planet
    CelestialBody* earth = new CelestialBody("Earth", 1.0f, 0.5f);
    earth->getPhysicsBody().position = Vec3(10, 0, 0);
    earth->getPhysicsBody().velocity = Vec3(0, 3.0f, 0);

    // Mars-like Planet
    CelestialBody* mars = new CelestialBody("Mars", 0.8f, 0.4f);
    mars->getPhysicsBody().position = Vec3(15, 0, 0);
    mars->getPhysicsBody().velocity = Vec3(0, 2.5f, 0);

    // Jupiter-like Gas Giant
    CelestialBody* jupiter = new CelestialBody("Jupiter", 5.0f, 1.2f);
    jupiter->getPhysicsBody().position = Vec3(25, 0, 0);
    jupiter->getPhysicsBody().velocity = Vec3(0, 1.8f, 0);

    // Black Hole (far from system)
    CelestialBody* blackHole = new CelestialBody("Black Hole", 5000.0f, 1.5f);
    blackHole->getPhysicsBody().position = Vec3(-40, 20, 0);
    blackHole->getPhysicsBody().velocity = Vec3(0, 0, 0);

    // Small asteroid between Mars and Jupiter
    CelestialBody* asteroid = new CelestialBody("Asteroid", 0.1f, 0.15f);
    asteroid->getPhysicsBody().position = Vec3(20, 0, 5);
    asteroid->getPhysicsBody().velocity = Vec3(-0.5f, 2.2f, 0.3f);

    system.push_back(sun);
    system.push_back(mercury);
    system.push_back(earth);
    system.push_back(mars);
    system.push_back(jupiter);
    system.push_back(blackHole);
    system.push_back(asteroid);

    return system;
}