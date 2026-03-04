#include "SolarSystemFactory.hpp"

std::vector<CelestialBody*> SolarSystemFactory::createSimpleSystem()
{
    std::vector<CelestialBody*> system;

    // Sun - Bright yellow-orange glow
    CelestialBody* sun = new CelestialBody("Sun", 2000.0f, 3.5f, Vec3(1.0f, 0.9f, 0.2f));
    sun->getPhysicsBody().position = Vec3(0, 0, 0);
    sun->getPhysicsBody().velocity = Vec3(0, 0, 0);

    // Mercury - Small gray rocky planet
    CelestialBody* mercury = new CelestialBody("Mercury", 0.4f, 0.35f, Vec3(0.5f, 0.5f, 0.5f));
    mercury->getPhysicsBody().position = Vec3(8, 0, 0);
    mercury->getPhysicsBody().velocity = Vec3(0, 5.2f, 0);

    // Venus - Yellowish cloudy atmosphere
    CelestialBody* venus = new CelestialBody("Venus", 0.85f, 0.55f, Vec3(0.95f, 0.85f, 0.5f));
    venus->getPhysicsBody().position = Vec3(12, 0, 0);
    venus->getPhysicsBody().velocity = Vec3(0, 4.3f, 0);

    // Earth - Blue oceans, green continents
    CelestialBody* earth = new CelestialBody("Earth", 1.0f, 0.6f, Vec3(0.2f, 0.5f, 0.95f));
    earth->getPhysicsBody().position = Vec3(18, 0, 0);
    earth->getPhysicsBody().velocity = Vec3(0, 3.6f, 0);

    // Mars - Red dusty terrain
    CelestialBody* mars = new CelestialBody("Mars", 0.6f, 0.45f, Vec3(0.85f, 0.35f, 0.2f));
    mars->getPhysicsBody().position = Vec3(24, 0, 0);
    mars->getPhysicsBody().velocity = Vec3(0, 3.0f, 0);

    // Jupiter - Massive gas giant with orange/white storms
    CelestialBody* jupiter = new CelestialBody("Jupiter", 15.0f, 2.2f, Vec3(0.85f, 0.65f, 0.4f));
    jupiter->getPhysicsBody().position = Vec3(38, 0, 0);
    jupiter->getPhysicsBody().velocity = Vec3(0, 2.2f, 0);

    // Saturn - Large with golden hue (rings not rendered yet)
    CelestialBody* saturn = new CelestialBody("Saturn", 12.0f, 2.0f, Vec3(0.9f, 0.8f, 0.6f));
    saturn->getPhysicsBody().position = Vec3(52, 0, 0);
    saturn->getPhysicsBody().velocity = Vec3(0, 1.8f, 0);

    // Uranus - Pale cyan ice giant
    CelestialBody* uranus = new CelestialBody("Uranus", 4.5f, 1.0f, Vec3(0.5f, 0.85f, 0.9f));
    uranus->getPhysicsBody().position = Vec3(68, 0, 0);
    uranus->getPhysicsBody().velocity = Vec3(0, 1.4f, 0);

    // Neptune - Deep blue with storms
    CelestialBody* neptune = new CelestialBody("Neptune", 4.3f, 0.95f, Vec3(0.2f, 0.3f, 0.8f));
    neptune->getPhysicsBody().position = Vec3(82, 0, 0);
    neptune->getPhysicsBody().velocity = Vec3(0, 1.2f, 0);

    // Black Hole - Dark purple/black with event horizon
    CelestialBody* blackHole = new CelestialBody("Black Hole", 8000.0f, 2.0f, Vec3(0.1f, 0.0f, 0.2f));
    blackHole->getPhysicsBody().position = Vec3(-100, 30, 0);
    blackHole->getPhysicsBody().velocity = Vec3(0, 0, 0);

    // Asteroid Belt - Several small asteroids
    CelestialBody* asteroid1 = new CelestialBody("Asteroid-1", 0.05f, 0.12f, Vec3(0.6f, 0.5f, 0.4f));
    asteroid1->getPhysicsBody().position = Vec3(28, 0, 3);
    asteroid1->getPhysicsBody().velocity = Vec3(-0.3f, 2.8f, 0.2f);

    CelestialBody* asteroid2 = new CelestialBody("Asteroid-2", 0.05f, 0.1f, Vec3(0.55f, 0.45f, 0.38f));
    asteroid2->getPhysicsBody().position = Vec3(30, 0, -4);
    asteroid2->getPhysicsBody().velocity = Vec3(0.2f, 2.7f, -0.15f);

    CelestialBody* asteroid3 = new CelestialBody("Asteroid-3", 0.06f, 0.13f, Vec3(0.65f, 0.55f, 0.42f));
    asteroid3->getPhysicsBody().position = Vec3(27, 0, 0);
    asteroid3->getPhysicsBody().velocity = Vec3(0, 2.9f, 0.25f);

    // Add all bodies to system
    system.push_back(sun);
    system.push_back(mercury);
    system.push_back(venus);
    system.push_back(earth);
    system.push_back(mars);
    system.push_back(jupiter);
    system.push_back(saturn);
    system.push_back(uranus);
    system.push_back(neptune);
    system.push_back(blackHole);
    system.push_back(asteroid1);
    system.push_back(asteroid2);
    system.push_back(asteroid3);

    return system;
}