#pragma once
#include "Vec3.hpp"
#include "Body.hpp"
#include <vector>
#include <string>

// Represents a calculated Hohmann transfer orbit between two bodies
struct TransferOrbit
{
    Vec3 departurePoint;      // Where to start the burn
    Vec3 arrivalPoint;        // Where you'll arrive
    Vec3 burnVelocity;        // Velocity change needed (Delta-V)
    float transferTime;       // How long the transfer takes (seconds)
    float deltaV;             // Total Delta-V required
    float phase_angle;        // Current phase angle between bodies
    float optimal_phase;      // Optimal phase angle for transfer
    bool isValid;             // Whether transfer is possible
    std::vector<Vec3> transferPath;  // Points along the transfer orbit
};

// Calculates optimal Hohmann transfer orbits between celestial bodies
// This is the REAL math NASA uses for interplanetary missions!
class HohmannTransfer
{
public:
    HohmannTransfer();

    // Calculate transfer from one body to another
    TransferOrbit calculateTransfer(const Body& from, const Body& to,
                                   const Body& centralBody, float currentTime);

    // Calculate where to place burn maneuver nodes
    Vec3 calculateBurnPoint(const Body& from, const Body& centralBody);

    // Calculate Delta-V required for transfer
    float calculateDeltaV(const Body& from, const Body& to, const Body& centralBody);

    // Calculate optimal phase angle between planets
    float calculateOptimalPhaseAngle(const Body& from, const Body& to, const Body& centralBody);

    // Check if transfer window is open (planets in good position)
    bool isTransferWindowOpen(const Body& from, const Body& to,
                             const Body& centralBody, float tolerance = 15.0f);

    void setActive(bool active) { isActive = active; }
    bool getActive() const { return isActive; }

    const TransferOrbit& getCurrentTransfer() const { return currentTransfer; }

private:
    float calculateOrbitalRadius(const Vec3& position, const Vec3& centralPos);
    float calculateOrbitalVelocity(float radius, float centralMass);
    float calculateTransferTime(float r1, float r2, float centralMass);
    void generateTransferPath(TransferOrbit& transfer, const Body& central);

    TransferOrbit currentTransfer;
    bool isActive;
};
