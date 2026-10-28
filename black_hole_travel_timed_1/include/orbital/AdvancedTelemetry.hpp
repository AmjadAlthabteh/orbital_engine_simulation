#pragma once

#include "Vec3.hpp"
#include "Spaceship.hpp"
#include <vector>
#include <iostream>
#include <iomanip>
#include <cmath>

class AdvancedTelemetry
{
private:
    // Historical data
    std::vector<float> accelerationHistory;
    std::vector<float> gForceHistory;
    std::vector<float> altitudeHistory;
    const int historySize = 60;  // 60 samples

    // Calculated values
    Vec3 lastVelocity;
    float maxGForce;
    float maxAcceleration;
    float avgGForce;

    // G-force calculation (1 G = 9.81 m/s^2)
    const float EARTH_GRAVITY = 9.81f;

public:
    AdvancedTelemetry() : lastVelocity(Vec3(0, 0, 0)), maxGForce(0.0f),
                         maxAcceleration(0.0f), avgGForce(0.0f)
    {
        accelerationHistory.resize(historySize, 0.0f);
        gForceHistory.resize(historySize, 0.0f);
        altitudeHistory.resize(historySize, 0.0f);
    }

    // Update telemetry data
    void update(const Spaceship& ship, float deltaTime, const std::vector<class CelestialBody*>& bodies)
    {
        if (deltaTime < 0.001f) return;  // Avoid division by zero

        Vec3 currentVel = ship.getPhysicsBody().velocity;

        // Calculate acceleration (change in velocity)
        Vec3 accelVec = Vec3(
            (currentVel.x - lastVelocity.x) / deltaTime,
            (currentVel.y - lastVelocity.y) / deltaTime,
            (currentVel.z - lastVelocity.z) / deltaTime
        );

        float acceleration = accelVec.length();
        float gForce = acceleration / EARTH_GRAVITY;

        // Update history
        accelerationHistory.erase(accelerationHistory.begin());
        accelerationHistory.push_back(acceleration);

        gForceHistory.erase(gForceHistory.begin());
        gForceHistory.push_back(gForce);

        // Calculate altitude
        float nearestDist = 999999.0f;
        Vec3 shipPos = ship.getPhysicsBody().position;

        for (auto* body : bodies)
        {
            Vec3 diff = body->getPhysicsBody().position - shipPos;
            float dist = diff.length();
            if (dist < nearestDist)
            {
                nearestDist = dist - body->getRadius();
            }
        }

        altitudeHistory.erase(altitudeHistory.begin());
        altitudeHistory.push_back(nearestDist);

        // Update max values
        if (gForce > maxGForce)
            maxGForce = gForce;
        if (acceleration > maxAcceleration)
            maxAcceleration = acceleration;

        // Calculate average G-force
        float sum = 0.0f;
        for (float g : gForceHistory)
            sum += g;
        const float sampleCount = static_cast<float>(gForceHistory.size());
        avgGForce = (sampleCount > 0.0f) ? (sum / sampleCount) : 0.0f;

        lastVelocity = currentVel;
    }

    // Display comprehensive telemetry
    void displayComprehensive(const Spaceship& ship, const std::vector<class CelestialBody*>& bodies) const
    {
        Vec3 pos = ship.getPhysicsBody().position;
        Vec3 vel = ship.getPhysicsBody().velocity;
        float speed = ship.getSpeed();

        // Find nearest body
        float nearestDist = 999999.0f;
        std::string nearestName = "Unknown";
        float nearestRadius = 0.0f;
        Vec3 nearestPos;

        for (auto* body : bodies)
        {
            Vec3 diff = body->getPhysicsBody().position - pos;
            float dist = diff.length();
            if (dist < nearestDist)
            {
                nearestDist = dist;
                nearestName = body->getName();
                nearestRadius = body->getRadius();
                nearestPos = body->getPhysicsBody().position;
            }
        }

        float altitude = nearestDist - nearestRadius;
        float currentGForce = gForceHistory.back();
        float currentAccel = accelerationHistory.back();

        std::cout << "\n╔═══════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║               ADVANCED TELEMETRY SYSTEM                           ║\n";
        std::cout << "╠═══════════════════════════════════════════════════════════════════╣\n";

        // Position & Velocity
        std::cout << "║  POSITION & MOTION                                                ║\n";
        std::cout << "║  ────────────────────────────────────────────────────────────────  ║\n";
        std::cout << "║  Position:     (" << std::fixed << std::setprecision(1)
                  << std::setw(8) << pos.x << ", "
                  << std::setw(8) << pos.y << ", "
                  << std::setw(8) << pos.z << ")              ║\n";
        std::cout << "║  Velocity:     (" << std::setw(8) << vel.x << ", "
                  << std::setw(8) << vel.y << ", "
                  << std::setw(8) << vel.z << ") m/s        ║\n";
        std::cout << "║  Speed:        " << std::setw(10) << speed << " m/s                                 ║\n";
        std::cout << "║                                                                   ║\n";

        // Acceleration & G-Forces
        std::cout << "║  ACCELERATION & FORCES                                            ║\n";
        std::cout << "║  ────────────────────────────────────────────────────────────────  ║\n";
        std::cout << "║  Current Accel: " << std::setw(8) << currentAccel << " m/s²                              ║\n";
        std::cout << "║  Current G's:   " << std::setw(8) << currentGForce << " G                                  ║\n";
        std::cout << "║  Max G's:       " << std::setw(8) << maxGForce << " G (Peak)                          ║\n";
        std::cout << "║  Avg G's:       " << std::setw(8) << avgGForce << " G (1 min avg)                     ║\n";

        // G-Force warning
        if (currentGForce > 8.0f)
        {
            std::cout << "║  ⚠️  EXTREME G-FORCES! PILOT UNCONSCIOUSNESS RISK!               ║\n";
        }
        else if (currentGForce > 5.0f)
        {
            std::cout << "║  ⚠️  HIGH G-FORCES! Stress on ship structure                     ║\n";
        }

        std::cout << "║                                                                   ║\n";

        // Proximity & Altitude
        std::cout << "║  PROXIMITY ANALYSIS                                               ║\n";
        std::cout << "║  ────────────────────────────────────────────────────────────────  ║\n";
        std::cout << "║  Nearest Body:  " << std::left << std::setw(48) << nearestName << "║\n" << std::right;
        std::cout << "║  Distance:      " << std::setw(10) << nearestDist << " units                             ║\n";
        std::cout << "║  Altitude:      " << std::setw(10) << altitude << " units                             ║\n";

        if (altitude < 5.0f)
        {
            std::cout << "║  🔴 COLLISION WARNING! PULL UP IMMEDIATELY!                       ║\n";
        }
        else if (altitude < 15.0f)
        {
            std::cout << "║  🟡 CAUTION: Low altitude detected                                ║\n";
        }
        else if (altitude < 30.0f)
        {
            std::cout << "║  🟢 Altitude safe for operations                                  ║\n";
        }

        std::cout << "║                                                                   ║\n";

        // Orbital mechanics (simplified)
        Vec3 toNearest = Vec3(
            nearestPos.x - pos.x,
            nearestPos.y - pos.y,
            nearestPos.z - pos.z
        );
        float distToNearest = toNearest.length();
        Vec3 radialDir = Vec3(
            toNearest.x / distToNearest,
            toNearest.y / distToNearest,
            toNearest.z / distToNearest
        );

        // Radial and tangential velocity components
        float radialVel = vel.x * radialDir.x + vel.y * radialDir.y + vel.z * radialDir.z;
        Vec3 tangentialVelVec = Vec3(
            vel.x - radialVel * radialDir.x,
            vel.y - radialVel * radialDir.y,
            vel.z - radialVel * radialDir.z
        );
        float tangentialVel = tangentialVelVec.length();

        std::cout << "║  ORBITAL PARAMETERS (relative to " << std::left << std::setw(18) << nearestName << ")    ║\n" << std::right;
        std::cout << "║  ────────────────────────────────────────────────────────────────  ║\n";
        std::cout << "║  Radial Velocity:    " << std::setw(10) << radialVel << " m/s                        ║\n";
        std::cout << "║  Tangential Vel:     " << std::setw(10) << tangentialVel << " m/s                        ║\n";

        if (radialVel < -5.0f)
        {
            std::cout << "║  Status: DESCENDING (approaching body)                            ║\n";
        }
        else if (radialVel > 5.0f)
        {
            std::cout << "║  Status: ASCENDING (moving away)                                 ║\n";
        }
        else
        {
            std::cout << "║  Status: STABLE (maintaining distance)                           ║\n";
        }

        std::cout << "╚═══════════════════════════════════════════════════════════════════╝\n";
    }

    // Get current G-force
    float getCurrentGForce() const
    {
        return gForceHistory.empty() ? 0.0f : gForceHistory.back();
    }

    // Get max G-force
    float getMaxGForce() const { return maxGForce; }

    // Get average G-force
    float getAvgGForce() const { return avgGForce; }

    // Reset max values
    void resetMaxValues()
    {
        maxGForce = 0.0f;
        maxAcceleration = 0.0f;
    }
};
