#include "OrbitalMechanicsHUD.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>

const float PI = 3.14159265359f;
const float G_SCALED = 0.1f;  // Gravitational constant (scaled for simulation)

OrbitalMechanicsHUD::OrbitalMechanicsHUD()
    : isEnabled(true), updateInterval(2.0f), timeSinceLastUpdate(0.0f)
{
}

OrbitalParameters OrbitalMechanicsHUD::calculateOrbitalParameters(const Body& orbiting,
                                                                   const Body& central)
{
    OrbitalParameters params;

    Vec3 relPos = orbiting.position - central.position;
    Vec3 relVel = orbiting.velocity - central.velocity;

    float r = relPos.length();
    float v = relVel.length();

    if (r < 0.01f)
    {
        // Too close, invalid orbit
        params.isStable = false;
        params.orbitType = "N/A";
        return params;
    }

    // Calculate eccentricity
    params.eccentricity = calculateEccentricity(relPos, relVel, Vec3(0,0,0), central.mass);

    // Calculate semi-major axis
    params.semiMajorAxis = calculateSemiMajorAxis(relPos, relVel, Vec3(0,0,0), central.mass);

    // Calculate semi-minor axis: b = a * sqrt(1 - e²)
    params.semiMinorAxis = params.semiMajorAxis * std::sqrt(1.0f - params.eccentricity * params.eccentricity);

    // Calculate apoapsis and periapsis
    params.apoapsis = params.semiMajorAxis * (1.0f + params.eccentricity);
    params.periapsis = params.semiMajorAxis * (1.0f - params.eccentricity);

    // Current altitude (distance from surface, assuming central body has radius)
    params.altitude = r;  // Simplified - would subtract planet radius in reality

    // Current orbital velocity
    params.orbitalVelocity = v;

    // Orbital period (for elliptical orbits)
    if (params.eccentricity < 1.0f)
    {
        params.orbitalPeriod = calculateOrbitalPeriod(params.semiMajorAxis, central.mass);
        params.isStable = true;
    }
    else
    {
        params.orbitalPeriod = 0.0f;  // Hyperbolic/parabolic - no period
        params.isStable = false;
    }

    // Specific orbital energy: E = v²/2 - GM/r
    float GM = central.mass * G_SCALED;
    params.specificEnergy = (v * v) / 2.0f - GM / r;

    // Angular momentum magnitude: L = r × v
    Vec3 angularMomentumVec = relPos.cross(relVel);
    params.angularMomentum = angularMomentumVec.length();

    // Inclination (angle from XZ plane)
    Vec3 normalVec = angularMomentumVec.normalize();
    params.inclination = std::acos(std::abs(normalVec.y)) * 180.0f / PI;

    // Classify orbit type
    params.orbitType = classifyOrbitType(params.eccentricity);

    return params;
}

float OrbitalMechanicsHUD::calculateEccentricity(const Vec3& position, const Vec3& velocity,
                                                 const Vec3& centralPos, float centralMass)
{
    Vec3 r = position - centralPos;
    Vec3 v = velocity;

    float r_mag = r.length();
    float v_mag = v.length();

    if (r_mag < 0.01f) return 0.0f;

    float GM = centralMass * G_SCALED;

    // Eccentricity vector: e = (v × L)/GM - r/|r|
    Vec3 L = r.cross(v);  // Angular momentum
    Vec3 e_vec = (v.cross(L)) * (1.0f / GM) - r * (1.0f / r_mag);

    return e_vec.length();
}

float OrbitalMechanicsHUD::calculateSemiMajorAxis(const Vec3& position, const Vec3& velocity,
                                                  const Vec3& centralPos, float centralMass)
{
    Vec3 r = position - centralPos;
    Vec3 v = velocity;

    float r_mag = r.length();
    float v_mag = v.length();

    float GM = centralMass * G_SCALED;

    // Semi-major axis: a = -GM / (2E) where E is specific orbital energy
    float specificEnergy = (v_mag * v_mag) / 2.0f - GM / r_mag;

    if (specificEnergy >= 0.0f)
    {
        // Parabolic or hyperbolic - no closed orbit
        return 0.0f;
    }

    return -GM / (2.0f * specificEnergy);
}

float OrbitalMechanicsHUD::calculateOrbitalPeriod(float semiMajorAxis, float centralMass)
{
    if (semiMajorAxis <= 0.0f) return 0.0f;

    float GM = centralMass * G_SCALED;

    // Kepler's third law: T = 2π * sqrt(a³ / GM)
    float period = 2.0f * PI * std::sqrt((semiMajorAxis * semiMajorAxis * semiMajorAxis) / GM);

    return period;
}

std::string OrbitalMechanicsHUD::classifyOrbitType(float eccentricity)
{
    if (eccentricity < 0.05f)
        return "Circular";
    else if (eccentricity < 1.0f)
        return "Elliptical";
    else if (std::abs(eccentricity - 1.0f) < 0.01f)
        return "Parabolic";
    else
        return "Hyperbolic";
}

void OrbitalMechanicsHUD::displayToConsole(const OrbitalParameters& params,
                                           const std::string& bodyName,
                                           const std::string& centralName)
{
    if (!isEnabled)
        return;

    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║           ORBITAL MECHANICS - " << bodyName << " around " << centralName << std::string(20 - bodyName.length() - centralName.length(), ' ') << "║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════════╣\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "║  Orbit Type:        " << std::left << std::setw(35) << params.orbitType << "║\n";
    std::cout << "║  Eccentricity:      " << std::left << std::setw(35) << params.eccentricity << "║\n";
    std::cout << "║  Semi-Major Axis:   " << std::left << std::setw(35) << (std::to_string(params.semiMajorAxis) + " units") << "║\n";
    std::cout << "║  Apoapsis:          " << std::left << std::setw(35) << (std::to_string(params.apoapsis) + " units") << "║\n";
    std::cout << "║  Periapsis:         " << std::left << std::setw(35) << (std::to_string(params.periapsis) + " units") << "║\n";

    if (params.isStable)
    {
        std::cout << "║  Orbital Period:    " << std::left << std::setw(35) << (std::to_string(params.orbitalPeriod) + " sec") << "║\n";
    }
    else
    {
        std::cout << "║  Orbital Period:    " << std::left << std::setw(35) << "N/A (Escape trajectory!)" << "║\n";
    }

    std::cout << "║  Altitude:          " << std::left << std::setw(35) << (std::to_string(params.altitude) + " units") << "║\n";
    std::cout << "║  Velocity:          " << std::left << std::setw(35) << (std::to_string(params.orbitalVelocity) + " units/sec") << "║\n";
    std::cout << "║  Inclination:       " << std::left << std::setw(35) << (std::to_string(params.inclination) + " degrees") << "║\n";
    std::cout << "║  Specific Energy:   " << std::left << std::setw(35) << params.specificEnergy << "║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
}

void OrbitalMechanicsHUD::displayTransferInfo(float deltaV, float transferTime,
                                             float phaseAngle, float optimalPhase)
{
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              HOHMANN TRANSFER CALCULATOR                      ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════════╣\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "║  Total Delta-V:     " << std::left << std::setw(35) << (std::to_string(deltaV) + " units/sec") << "║\n";
    std::cout << "║  Transfer Time:     " << std::left << std::setw(35) << (std::to_string(transferTime) + " seconds") << "║\n";
    std::cout << "║  Current Phase:     " << std::left << std::setw(35) << (std::to_string(phaseAngle) + " degrees") << "║\n";
    std::cout << "║  Optimal Phase:     " << std::left << std::setw(35) << (std::to_string(optimalPhase) + " degrees") << "║\n";

    float angleDiff = std::abs(phaseAngle - optimalPhase);
    if (angleDiff < 10.0f)
    {
        std::cout << "║  Transfer Window:   " << std::left << std::setw(35) << "OPEN! ✓ Good to go!" << "║\n";
    }
    else
    {
        std::cout << "║  Transfer Window:   " << std::left << std::setw(35) << ("CLOSED (wait " + std::to_string(angleDiff) + "°)") << "║\n";
    }

    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
}

void OrbitalMechanicsHUD::displayLagrangeInfo(int numPoints, bool showDetails)
{
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════════╗\n";
    std::cout << "║              LAGRANGE POINTS DETECTED                         ║\n";
    std::cout << "╠═══════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  Total Points:      " << std::left << std::setw(35) << numPoints << "║\n";
    std::cout << "║  L1, L2, L3:        Unstable (yellow/orange markers)          ║\n";
    std::cout << "║  L4, L5:            STABLE! (green markers - park here!)      ║\n";
    std::cout << "║                                                               ║\n";
    std::cout << "║  These are gravitational 'sweet spots' where objects can     ║\n";
    std::cout << "║  maintain a stable position relative to two larger bodies!   ║\n";
    std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
}
