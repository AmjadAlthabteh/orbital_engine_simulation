#pragma once
#include <string>
#include <vector>
#include "Vec3.hpp"
#include "Body.hpp"

// Orbital parameters calculated from physics
struct OrbitalParameters
{
    float apoapsis;          // Highest point in orbit (km)
    float periapsis;         // Lowest point in orbit (km)
    float eccentricity;      // How elliptical (0 = circle, 0.9 = very elliptical)
    float semiMajorAxis;     // Half the long axis of ellipse
    float semiMinorAxis;     // Half the short axis of ellipse
    float orbitalPeriod;     // Time for one complete orbit (seconds)
    float orbitalVelocity;   // Current speed in orbit
    float altitude;          // Current altitude above surface
    float inclination;       // Orbit tilt angle (degrees)
    float specificEnergy;    // Orbital energy per unit mass
    float angularMomentum;   // Rotational momentum

    std::string orbitType;   // "Circular", "Elliptical", "Parabolic", "Hyperbolic"
    bool isStable;           // Whether orbit is closed/stable
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
