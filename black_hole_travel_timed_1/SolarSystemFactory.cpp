#include "SolarSystemFactory.hpp"
#include <cmath>

static float G = 0.001f;

void SolarSystemFactory::createSolarSystem(std::vector<CelestialBody>& bodies)
{
    bodies.clear();

    float sunMass = 50000.f;

    bodies.emplace_back(
        Vec3(0, 0, 0),
        Vec3(0, 0, 0),
        3.0f,
        sunMass,
        Vec3(1.0f, 0.9f, 0.2f)
    );

    auto createPlanet = [&](float distance, float mass, float radius, Vec3 color)
        {
            float orbitalVelocity = std::sqrt(G * sunMass / distance);

            bodies.emplace_back(
                Vec3(distance, 0, 0),
                Vec3(0, 0, -orbitalVelocity),
                radius,
                mass,
                color
            );
        };

    createPlanet(8.f, 5.f, 0.5f, Vec3(0.5f, 0.5f, 0.5f)); // Mercury
    createPlanet(12.f, 8.f, 0.7f, Vec3(0.9f, 0.6f, 0.2f)); // Venus
    createPlanet(16.f, 10.f, 0.8f, Vec3(0.2f, 0.6f, 1.0f)); // Earth
    createPlanet(22.f, 7.f, 0.6f, Vec3(1.0f, 0.3f, 0.2f)); // Mars
}