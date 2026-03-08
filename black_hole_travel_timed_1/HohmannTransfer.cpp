#include "HohmannTransfer.hpp"
#include <cmath>
#include <iostream>

const float PI = 3.14159265359f;
const float G = 6.67430e-11f;  // Gravitational constant (scaled for our sim)

HohmannTransfer::HohmannTransfer() : isActive(false)
{
    currentTransfer.isValid = false;
}

TransferOrbit HohmannTransfer::calculateTransfer(const Body& from, const Body& to,
                                                 const Body& centralBody, float currentTime)
{
    TransferOrbit transfer;
    transfer.isValid = false;

    Vec3 fromPos = from.position - centralBody.position;
    Vec3 toPos = to.position - centralBody.position;

    float r1 = fromPos.length();
    float r2 = toPos.length();

    if (r1 < 0.1f || r2 < 0.1f)
    {
        return transfer;  // Invalid - too close to central body
    }

    // Calculate orbital velocities
    float v1 = calculateOrbitalVelocity(r1, centralBody.mass);
    float v2 = calculateOrbitalVelocity(r2, centralBody.mass);

    // Hohmann transfer velocities
    // At periapsis (inner orbit): v_transfer = sqrt(GM * (2/r1 - 2/(r1+r2)))
    // At apoapsis (outer orbit): v_transfer = sqrt(GM * (2/r2 - 2/(r1+r2)))
    float GM = centralBody.mass * 0.1f;  // Scaled for our simulation

    float v_transfer_at_r1 = std::sqrt(GM * (2.0f/r1 - 2.0f/(r1+r2)));
    float v_transfer_at_r2 = std::sqrt(GM * (2.0f/r2 - 2.0f/(r1+r2)));

    // Delta-V calculations
    float deltaV1 = std::abs(v_transfer_at_r1 - v1);  // Burn at departure
    float deltaV2 = std::abs(v2 - v_transfer_at_r2);  // Burn at arrival
    transfer.deltaV = deltaV1 + deltaV2;

    // Transfer time (half period of transfer ellipse)
    transfer.transferTime = calculateTransferTime(r1, r2, centralBody.mass);

    // Calculate phase angle
    Vec3 fromDir = fromPos.normalize();
    Vec3 toDir = toPos.normalize();
    float dotProduct = fromDir.dot(toDir);
    dotProduct = std::max(-1.0f, std::min(1.0f, dotProduct));  // Clamp
    transfer.phase_angle = std::acos(dotProduct) * 180.0f / PI;

    // Calculate optimal phase angle
    transfer.optimal_phase = calculateOptimalPhaseAngle(from, to, centralBody);

    // Departure and arrival points
    transfer.departurePoint = from.position;

    // Calculate arrival point (where target will be after transfer time)
    float targetAngularVelocity = v2 / r2;  // radians per second
    float angleChange = targetAngularVelocity * transfer.transferTime;

    // Rotate target position by angle change
    float cosAngle = std::cos(angleChange);
    float sinAngle = std::sin(angleChange);
    Vec3 toPosRotated(
        toPos.x * cosAngle - toPos.z * sinAngle,
        toPos.y,
        toPos.x * sinAngle + toPos.z * cosAngle
    );
    transfer.arrivalPoint = toPosRotated + centralBody.position;

    // Burn velocity direction
    Vec3 velocityDir = from.velocity.normalize();
    transfer.burnVelocity = velocityDir * deltaV1;

    // Generate visual path
    generateTransferPath(transfer, centralBody);

    transfer.isValid = true;
    currentTransfer = transfer;

    return transfer;
}

float HohmannTransfer::calculateOrbitalRadius(const Vec3& position, const Vec3& centralPos)
{
    Vec3 diff = position - centralPos;
    return diff.length();
}

float HohmannTransfer::calculateOrbitalVelocity(float radius, float centralMass)
{
    if (radius < 0.1f) return 0.0f;
    float GM = centralMass * 0.1f;  // Scaled
    return std::sqrt(GM / radius);
}

float HohmannTransfer::calculateTransferTime(float r1, float r2, float centralMass)
{
    float GM = centralMass * 0.1f;  // Scaled
    float a = (r1 + r2) / 2.0f;  // Semi-major axis of transfer ellipse

    // Orbital period: T = 2π * sqrt(a³ / GM)
    float period = 2.0f * PI * std::sqrt((a * a * a) / GM);

    // Transfer time is half the period
    return period / 2.0f;
}

float HohmannTransfer::calculateOptimalPhaseAngle(const Body& from, const Body& to,
                                                   const Body& centralBody)
{
    Vec3 fromPos = from.position - centralBody.position;
    Vec3 toPos = to.position - centralBody.position;

    float r1 = fromPos.length();
    float r2 = toPos.length();

    float transferTime = calculateTransferTime(r1, r2, centralBody.mass);

    // Angular velocity of target
    float v2 = calculateOrbitalVelocity(r2, centralBody.mass);
    float omega2 = v2 / r2;  // radians per second

    // How far target will move during transfer
    float angleTarget = omega2 * transferTime;

    // Optimal phase angle = 180° - angle_target (in degrees)
    float optimalAngle = 180.0f - (angleTarget * 180.0f / PI);

    return optimalAngle;
}

bool HohmannTransfer::isTransferWindowOpen(const Body& from, const Body& to,
                                           const Body& centralBody, float tolerance)
{
    TransferOrbit transfer = calculateTransfer(from, to, centralBody, 0.0f);

    if (!transfer.isValid)
        return false;

    float angleDiff = std::abs(transfer.phase_angle - transfer.optimal_phase);

    return angleDiff < tolerance;
}

Vec3 HohmannTransfer::calculateBurnPoint(const Body& from, const Body& centralBody)
{
    // Burn happens at current position for simplicity
    // In reality, you'd calculate the optimal point along the orbit
    return from.position;
}

float HohmannTransfer::calculateDeltaV(const Body& from, const Body& to, const Body& centralBody)
{
    TransferOrbit transfer = calculateTransfer(from, to, centralBody, 0.0f);
    return transfer.deltaV;
}

void HohmannTransfer::generateTransferPath(TransferOrbit& transfer, const Body& central)
{
    transfer.transferPath.clear();

    Vec3 start = transfer.departurePoint - central.position;
    Vec3 end = transfer.arrivalPoint - central.position;

    float r1 = start.length();
    float r2 = end.length();
    float a = (r1 + r2) / 2.0f;  // Semi-major axis
    float e = (r2 - r1) / (r2 + r1);  // Eccentricity

    // Generate elliptical path points
    int numPoints = 100;
    for (int i = 0; i <= numPoints; i++)
    {
        float t = (float)i / (float)numPoints;
        float angle = t * PI;  // 0 to π (half ellipse)

        // Ellipse equation in polar form: r = a(1-e²) / (1 + e*cos(θ))
        float r = a * (1.0f - e * e) / (1.0f + e * std::cos(angle));

        // Calculate point on transfer ellipse
        float baseAngle = std::atan2(start.z, start.x);
        float x = r * std::cos(angle + baseAngle);
        float z = r * std::sin(angle + baseAngle);
        float y = start.y * (1.0f - t) + end.y * t;  // Interpolate y

        transfer.transferPath.push_back(Vec3(x, y, z) + central.position);
    }
}
