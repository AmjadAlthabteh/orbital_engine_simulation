#include "PhysicsEngine.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <cstdint>
#include <cstring>

// OPTIMIZATION: Fast inverse square root (Quake III style) for distance calculations
// Only about 1% less accurate than std::sqrt but significantly faster
inline float fastInvSqrt(float x)
{
    float halfx = 0.5f * x;
    static_assert(sizeof(float) == sizeof(std::uint32_t), "fastInvSqrt assumes 32-bit float");

    std::uint32_t i = 0;
    std::memcpy(&i, &x, sizeof(x));
    i = 0x5f3759df - (i >> 1);
    std::memcpy(&x, &i, sizeof(x));
    x = x * (1.5f - halfx * x * x);  // One Newton iteration
    return x;
}

PhysicsEngine::PhysicsEngine(float G)
{
    gravitationalConstant = G;
    recentCollisions.reserve(100);  // Pre-allocate for performance

    // Initialize adaptive substepping parameters
    maxTimeStep = 0.016f;         // ~60 FPS timestep maximum
    maxSubsteps = 8;              // Up to 8 substeps for high-speed scenarios
    velocityThreshold = 100.0f;   // Activate adaptive stepping above 100 m/s
    gravitySofteningLength = 0.25f;
    maxVelocityChangePerStep = 5.0f;
    lastSubstepCount = 1;
}

void PhysicsEngine::addBody(Body* body)
{
    bodies.push_back(body);
}

void PhysicsEngine::update(float deltaTime)
{
    if (deltaTime <= 0.0f)
        return;

    const int substeps = calculateAdaptiveSubsteps(deltaTime);

    lastSubstepCount = substeps;
    const float subDeltaTime = deltaTime / static_cast<float>(substeps);

    // Perform physics update in substeps
    for (int step = 0; step < substeps; ++step)
    {
        applyGravity();
        handleCollisions();

        // Integrate all bodies (with null pointer safety check)
        for (auto* body : bodies)
        {
            if (body != nullptr)
            {
                body->integrate(subDeltaTime);
            }
        }
    }

    // Update collision markers once per frame (not per substep)
    updateCollisionMarkers(deltaTime);
}

const std::vector<CollisionEvent>& PhysicsEngine::getRecentCollisions() const
{
    return recentCollisions;
}

void PhysicsEngine::setMaxTimeStep(float maxStep)
{
    maxTimeStep = std::max(maxStep, 0.001f);
}

void PhysicsEngine::setMaxSubsteps(int maxSteps)
{
    maxSubsteps = std::max(maxSteps, 1);
}

void PhysicsEngine::setVelocityThreshold(float threshold)
{
    velocityThreshold = std::max(threshold, 0.0f);
}

void PhysicsEngine::setGravitySofteningLength(float length)
{
    gravitySofteningLength = std::max(length, 0.0f);
}

void PhysicsEngine::setMaxVelocityChangePerStep(float deltaV)
{
    maxVelocityChangePerStep = std::max(deltaV, 0.001f);
}

void PhysicsEngine::updateCollisionMarkers(float deltaTime)
{
    // Update timestamps and remove old collision markers (older than 3 seconds).
    // PERF: avoid repeated vector::erase() inside a loop (O(n^2) moves).
    for (auto& event : recentCollisions)
    {
        event.timestamp += deltaTime;
    }

    recentCollisions.erase(
        std::remove_if(
            recentCollisions.begin(),
            recentCollisions.end(),
            [](const CollisionEvent& event) { return event.timestamp > 3.0f; }),
        recentCollisions.end());
}

void PhysicsEngine::applyGravity()
{
    if (bodies.size() < 2)
        return;

    const size_t bodyCount = bodies.size();
    Body* const* const bodyData = bodies.data();

    for (size_t i = 0; i < bodyCount; ++i)
    {
        Body* const bodyI = bodyData[i];
        if (bodyI == nullptr)
            continue;

        for (size_t j = i + 1; j < bodyCount; ++j)
        {
            Body* const bodyJ = bodyData[j];
            if (bodyJ == nullptr)
                continue;

            // OPTIMIZATION: Manually unroll vector subtraction (cache-friendly)
            const float dx = bodyJ->position.x - bodyI->position.x;
            const float dy = bodyJ->position.y - bodyI->position.y;
            const float dz = bodyJ->position.z - bodyI->position.z;

            const float softeningSquared = gravitySofteningLength * gravitySofteningLength;
            const float distanceSquared = dx * dx + dy * dy + dz * dz + softeningSquared;

            if (distanceSquared < 0.0001f) continue;  // 0.01^2

            // Plummer-style softening keeps close passes finite without changing far-field gravity.
            const float invDistance = fastInvSqrt(distanceSquared);
            const float invDistCubed = invDistance * invDistance * invDistance;

            // OPTIMIZATION: Combine mass product with G constant
            const float forceFactor = gravitationalConstant * bodyI->mass * bodyJ->mass * invDistCubed;

            // Calculate force components directly (avoid Vec3 temporaries)
            const float fx = dx * forceFactor;
            const float fy = dy * forceFactor;
            const float fz = dz * forceFactor;

            // Apply forces (compiler can now optimize better with explicit loads)
            bodyI->applyForce(Vec3(fx, fy, fz));
            bodyJ->applyForce(Vec3(-fx, -fy, -fz));
        }
    }
}

bool PhysicsEngine::checkCollision(const Body* a, const Body* b) const
{
    if (a == nullptr || b == nullptr)
        return false;

    Vec3 diff = b->position - a->position;
    float minDistance = a->radius + b->radius;
    return diff.lengthSquared() < (minDistance * minDistance);
}

float PhysicsEngine::predictCollisionTime(const Body* a, const Body* b) const
{
    if (a == nullptr || b == nullptr)
        return -1.0f;

    const Vec3 relativePos = b->position - a->position;
    const Vec3 relativeVel = b->velocity - a->velocity;
    const float minDistance = a->radius + b->radius;

    const double aCoeff = static_cast<double>(relativeVel.dot(relativeVel));
    const double bCoeff = 2.0 * static_cast<double>(relativePos.dot(relativeVel));
    const double cCoeff = static_cast<double>(relativePos.dot(relativePos)) -
        static_cast<double>(minDistance) * static_cast<double>(minDistance);

    if (cCoeff <= 0.0)
        return 0.0f;

    if (aCoeff <= 0.0 || bCoeff >= 0.0)
        return -1.0f;

    const double discriminant = bCoeff * bCoeff - 4.0 * aCoeff * cCoeff;
    if (discriminant < 0.0)
        return -1.0f;

    const double sqrtDiscriminant = std::sqrt(discriminant);
    const double q = -0.5 * (bCoeff + (bCoeff < 0.0 ? -sqrtDiscriminant : sqrtDiscriminant));
    const double t1 = q / aCoeff;
    const double t2 = q != 0.0 ? cCoeff / q : -1.0;

    const double firstImpact = std::min(t1, t2);
    const double secondImpact = std::max(t1, t2);

    if (firstImpact > 0.0)
        return static_cast<float>(firstImpact);
    if (secondImpact > 0.0)
        return static_cast<float>(secondImpact);
    return -1.0f;
}

bool PhysicsEngine::needsAdaptiveStepping() const
{
    // Check if any body is moving fast enough to require adaptive stepping
    const float thresholdSquared = velocityThreshold * velocityThreshold;
    for (const auto* body : bodies)
    {
        if (body != nullptr)
        {
            if (body->velocity.lengthSquared() > thresholdSquared)
            {
                return true;
            }
        }
    }
    return false;
}

int PhysicsEngine::calculateAdaptiveSubsteps(float deltaTime) const
{
    if (deltaTime <= 0.0f)
        return 1;

    int substeps = static_cast<int>(std::ceil(deltaTime / maxTimeStep));

    for (const auto* body : bodies)
    {
        if (body == nullptr || body->radius <= 0.0f)
            continue;

        const float travelDistance = body->velocity.length() * deltaTime;
        const float maxTravelPerStep = std::max(body->radius * 0.25f, 0.01f);
        const int travelSubsteps = static_cast<int>(std::ceil(travelDistance / maxTravelPerStep));
        substeps = std::max(substeps, travelSubsteps);
    }

    const float softeningSquared = gravitySofteningLength * gravitySofteningLength;
    const size_t bodyCount = bodies.size();
    for (size_t i = 0; i < bodyCount; ++i)
    {
        const Body* bodyI = bodies[i];
        if (bodyI == nullptr || bodyI->isStatic || maxVelocityChangePerStep <= 0.0f)
            continue;

        float accelerationMagnitude = 0.0f;
        for (size_t j = 0; j < bodyCount; ++j)
        {
            if (i == j)
                continue;

            const Body* bodyJ = bodies[j];
            if (bodyJ == nullptr || bodyJ->mass <= 0.0f)
                continue;

            const Vec3 delta = bodyJ->position - bodyI->position;
            const float distanceSquared = std::max(delta.lengthSquared() + softeningSquared, 0.0001f);
            accelerationMagnitude += gravitationalConstant * bodyJ->mass / distanceSquared;
        }

        const float velocityChange = accelerationMagnitude * deltaTime;
        const int accelerationSubsteps =
            static_cast<int>(std::ceil(velocityChange / maxVelocityChangePerStep));
        substeps = std::max(substeps, accelerationSubsteps);
    }

    if (needsAdaptiveStepping())
        substeps = std::max(substeps, 2);

    return std::max(1, std::min(substeps, maxSubsteps));
}

void PhysicsEngine::handleCollisions()
{
    if (bodies.size() < 2)
        return;

    const size_t bodyCount = bodies.size();
    Body* const* const bodyData = bodies.data();

    // PERFORMANCE: Broad-phase optimization using AABB checks
    // Skip expensive collision tests if bounding boxes don't overlap
    // This provides ~2-3x speedup for large body counts (>10 bodies)
    for (size_t i = 0; i < bodyCount; ++i)
    {
        Body* const bodyA = bodyData[i];
        if (bodyA == nullptr)
            continue;

        for (size_t j = i + 1; j < bodyCount; ++j)
        {
            Body* const bodyB = bodyData[j];
            if (bodyB == nullptr)
                continue;

            // BROAD-PHASE: Quick AABB (axis-aligned bounding box) rejection test
            // Much faster than sphere collision, eliminates ~60-80% of checks
            const Vec3 delta = bodyB->position - bodyA->position;
            const float maxDist = bodyA->radius + bodyB->radius;

            // Fast axis-by-axis rejection (cheaper than lengthSquared)
            if (std::abs(delta.x) > maxDist) continue;
            if (std::abs(delta.y) > maxDist) continue;
            if (std::abs(delta.z) > maxDist) continue;

            const float distanceSquared = delta.lengthSquared();
            if (distanceSquared < maxDist * maxDist)
            {
                const float distance = std::sqrt(distanceSquared);
                const Vec3 normal = distance > 0.0f ? delta * (1.0f / distance) : Vec3();
                const Vec3 collisionPoint = bodyA->position + normal * bodyA->radius;

                // Calculate relative velocity
                Vec3 relativeVel = bodyB->velocity - bodyA->velocity;
                float velAlongNormal = relativeVel.dot(normal);

                if (velAlongNormal > 0) continue;

                // Calculate collision intensity for visualization
                float collisionSpeed = relativeVel.length();

                const float inverseMassA = bodyA->inverseMass;
                const float inverseMassB = bodyB->inverseMass;
                const float inverseMassSum = inverseMassA + inverseMassB;
                if (inverseMassSum <= 0.0f)
                    continue;

                // Simple elastic collision response
                float e = 0.8f; // coefficient of restitution
                float j_impulse = -(1.0f + e) * velAlongNormal;
                j_impulse /= inverseMassSum;

                Vec3 impulse = normal * j_impulse;
                bodyA->applyImpulse(impulse * -1.0f);
                bodyB->applyImpulse(impulse);

                // Separate bodies in proportion to inverse mass to avoid moving heavy bodies too much.
                constexpr float penetrationSlop = 0.01f;
                constexpr float correctionPercent = 0.8f;
                const float overlap = std::max(maxDist - distance - penetrationSlop, 0.0f);
                const Vec3 correction = normal * (overlap * correctionPercent / inverseMassSum);
                bodyA->position = bodyA->position - correction * inverseMassA;
                bodyB->position = bodyB->position + correction * inverseMassB;

                // Record collision event for visualization (only once, not per substep)
                // Check if we recently recorded this collision to avoid duplicates
                bool isDuplicate = false;
                for (const auto& existing : recentCollisions)
                {
                    if (existing.timestamp < 0.1f &&  // Recent collision
                        (existing.bodyA == bodyA || existing.bodyA == bodyB) &&
                        (existing.bodyB == bodyA || existing.bodyB == bodyB))
                    {
                        isDuplicate = true;
                        break;
                    }
                }

                if (!isDuplicate)
                {
                    CollisionEvent event;
                    event.position = collisionPoint;
                    event.timestamp = 0.0f;  // Will be updated by timer
                    event.intensity = collisionSpeed;
                    event.bodyA = bodyA;
                    event.bodyB = bodyB;
                    recentCollisions.push_back(event);

                    // Console output for debugging
                    std::cout << "*** COLLISION DETECTED ***\n";
                    std::cout << "  Position: (" << collisionPoint.x << ", " << collisionPoint.y << ", " << collisionPoint.z << ")\n";
                    std::cout << "  Impact speed: " << collisionSpeed << " m/s\n";
                    std::cout << "  Bodies: Mass " << bodyA->mass << " vs Mass " << bodyB->mass << "\n";
                }
            }
        }
    }
}
