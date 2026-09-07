#pragma once
#include <string>
#include <vector>
#include "Vec3.hpp"
#include "Body.hpp"

// Orbital parameters calculated from physics
struct OrbitalParameters
{
    float apoapsis = 0.0f;          // Highest point in orbit (km)
    float periapsis = 0.0f;         // Lowest point in orbit (km)
    float eccentricity = 0.0f;      // How elliptical (0 = circle, 0.9 = very elliptical)
    float semiMajorAxis = 0.0f;     // Half the long axis of ellipse
    float semiMinorAxis = 0.0f;     // Half the short axis of ellipse
    float orbitalPeriod = 0.0f;     // Time for one complete orbit (seconds)
    float orbitalVelocity = 0.0f;   // Current speed in orbit
    float altitude = 0.0f;          // Current altitude above surface
    float inclination = 0.0f;       // Orbit tilt angle (degrees)
    float specificEnergy = 0.0f;    // Orbital energy per unit mass
    float angularMomentum = 0.0f;   // Rotational momentum

    std::string orbitType = "N/A";  // "Circular", "Elliptical", "Parabolic", "Hyperbolic"
    bool isStable = false;          // Whether orbit is closed/stable
};

// Console-based HUD that displays orbital mechanics data
// Shows real NASA-style orbital parameters!
class OrbitalMechanicsHUD
{
public:
    OrbitalMechanicsHUD();

    // Calculate orbital parameters for a body orbiting a central mass
    OrbitalParameters calculateOrbitalParameters(const Body& orbiting,
                                                  const Body& central);

    // Display HUD information to console
    void displayToConsole(const OrbitalParameters& params,
                         const std::string& bodyName,
                         const std::string& centralName);

    // Display transfer orbit information
    void displayTransferInfo(float deltaV, float transferTime,
                            float phaseAngle, float optimalPhase);

    // Display Lagrange points info
    void displayLagrangeInfo(int numPoints, bool showDetails);

    void setEnabled(bool enabled) { isEnabled = enabled; }
    bool getEnabled() const { return isEnabled; }

private:
    float calculateEccentricity(const Vec3& position, const Vec3& velocity,
                               const Vec3& centralPos, float centralMass);
    float calculateSemiMajorAxis(const Vec3& position, const Vec3& velocity,
                                const Vec3& centralPos, float centralMass);
    float calculateOrbitalPeriod(float semiMajorAxis, float centralMass);
    std::string classifyOrbitType(float eccentricity);

    bool isEnabled;
    float updateInterval;
    float timeSinceLastUpdate;
};
