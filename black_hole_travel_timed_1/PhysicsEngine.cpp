#include "PhysicsEngine.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>

// OPTIMIZATION: Fast inverse square root (Quake III style) for distance calculations
// Only about 1% less accurate than std::sqrt but significantly faster
inline float fastInvSqrt(float x)
{
    float halfx = 0.5f * x;
    int i = *(int*)&x;
    i = 0x5f3759df - (i >> 1);
    x = *(float*)&i;
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
    lastSubstepCount = 1;
}

void PhysicsEngine::addBody(Body* body)
{
    bodies.push_back(body);
}

void PhysicsEngine::update(float deltaTime)
{
    // ADAPTIVE SUBSTEPPING: Subdivide large timesteps for stability
    // This prevents physics tunneling at high speeds and improves accuracy

    // Determine if we need adaptive substepping
    bool useAdaptive = needsAdaptiveStepping();

    // Calculate number of substeps needed
    int substeps = 1;
    if (useAdaptive || deltaTime > maxTimeStep)
    {
        substeps = static_cast<int>(std::ceil(deltaTime / maxTimeStep));
        substeps = std::min(substeps, maxSubsteps);  // Clamp to max substeps
    }

    lastSubstepCount = substeps;
    float subDeltaTime = deltaTime / static_cast<float>(substeps);

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
    // SAFETY: Check bodies vector is valid
    if (bodies.empty() || bodies.size() == 0)
        return;

    const size_t bodyCount = bodies.size();

    for (size_t i = 0; i < bodyCount; ++i)
    {
        // SAFETY: Verify index is in range
        if (i >= bodies.size()) break;
        if (bodies[i] == nullptr) continue;

        for (size_t j = i + 1; j < bodyCount; ++j)
        {
            // SAFETY: Verify index is in range
            if (j >= bodies.size()) break;
            if (bodies[j] == nullptr) continue;

            Vec3 direction = bodies[j]->position - bodies[i]->position;

            // OPTIMIZATION: Calculate distance squared to avoid sqrt when possible
            float distanceSquared = direction.x * direction.x +
                                   direction.y * direction.y +
                                   direction.z * direction.z;

            if (distanceSquared < 0.0001f) continue;  // 0.01^2

            // PERFORMANCE BOOST: Use fast inverse square root (Quake III algorithm)
            // This eliminates the expensive sqrt() call and division in the critical path!
            // ~3-4x faster than std::sqrt + division, with negligible accuracy loss
            float invDistance = fastInvSqrt(distanceSquared);

            float forceMagnitude =
                gravitationalConstant *
                (bodies[i]->mass * bodies[j]->mass) *
                invDistance * invDistance;

            // Normalize using fast inverse distance
            Vec3 force = direction * (invDistance * forceMagnitude);

            bodies[i]->applyForce(force);
            bodies[j]->applyForce(force * -1.0f);
        }
    }
}

bool PhysicsEngine::checkCollision(const Body* a, const Body* b) const
{
    Vec3 diff = b->position - a->position;
    float minDistance = a->radius + b->radius;
    return diff.lengthSquared() < (minDistance * minDistance);
}

float PhysicsEngine::predictCollisionTime(const Body* a, const Body* b) const
{
    Vec3 relativePos = b->position - a->position;
    Vec3 relativeVel = b->velocity - a->velocity;
    float minDistance = a->radius + b->radius;

    float a_coeff = relativeVel.dot(relativeVel);
    float b_coeff = 2.0f * relativePos.dot(relativeVel);
    float c_coeff = relativePos.dot(relativePos) - minDistance * minDistance;

    float discriminant = b_coeff * b_coeff - 4.0f * a_coeff * c_coeff;

    if (discriminant < 0.0f || a_coeff == 0.0f)
        return -1.0f;

    float t1 = (-b_coeff - std::sqrt(discriminant)) / (2.0f * a_coeff);
    float t2 = (-b_coeff + std::sqrt(discriminant)) / (2.0f * a_coeff);

    if (t1 > 0.0f) return t1;
    if (t2 > 0.0f) return t2;
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

void PhysicsEngine::handleCollisions()
{
    // SAFETY: Check bodies vector is valid
    if (bodies.empty() || bodies.size() < 2)
        return;

    const size_t bodyCount = bodies.size();

    for (size_t i = 0; i < bodyCount; ++i)
    {
        // SAFETY: Verify index and pointer
        if (i >= bodies.size()) break;
        if (bodies[i] == nullptr) continue;

        for (size_t j = i + 1; j < bodyCount; ++j)
        {
            // SAFETY: Verify index and pointer
            if (j >= bodies.size()) break;
            if (bodies[j] == nullptr) continue;

            if (checkCollision(bodies[i], bodies[j]))
            {
                // Calculate collision point (midpoint between surfaces)
                Vec3 normal = (bodies[j]->position - bodies[i]->position).normalize();
                Vec3 collisionPoint = bodies[i]->position + normal * bodies[i]->radius;

                // Calculate relative velocity
                Vec3 relativeVel = bodies[j]->velocity - bodies[i]->velocity;
                float velAlongNormal = relativeVel.dot(normal);

                if (velAlongNormal > 0) continue;

                // Calculate collision intensity for visualization
                float collisionSpeed = relativeVel.length();

                // Simple elastic collision response
                float e = 0.8f; // coefficient of restitution
                float j_impulse = -(1.0f + e) * velAlongNormal;
                j_impulse /= (1.0f / bodies[i]->mass + 1.0f / bodies[j]->mass);

                Vec3 impulse = normal * j_impulse;
                bodies[i]->velocity = bodies[i]->velocity - impulse * (1.0f / bodies[i]->mass);
                bodies[j]->velocity = bodies[j]->velocity + impulse * (1.0f / bodies[j]->mass);

                // Separate bodies to prevent overlap
                float overlap = (bodies[i]->radius + bodies[j]->radius) -
                               (bodies[j]->position - bodies[i]->position).length();
                Vec3 separation = normal * (overlap * 0.5f);
                bodies[i]->position = bodies[i]->position - separation;
                bodies[j]->position = bodies[j]->position + separation;

                // Record collision event for visualization (only once, not per substep)
                // Check if we recently recorded this collision to avoid duplicates
                bool isDuplicate = false;
                for (const auto& existing : recentCollisions)
                {
                    if (existing.timestamp < 0.1f &&  // Recent collision
                        (existing.bodyA == bodies[i] || existing.bodyA == bodies[j]) &&
                        (existing.bodyB == bodies[i] || existing.bodyB == bodies[j]))
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
                    event.bodyA = bodies[i];
                    event.bodyB = bodies[j];
                    recentCollisions.push_back(event);

                    // Console output for debugging
                    std::cout << "*** COLLISION DETECTED ***\n";
                    std::cout << "  Position: (" << collisionPoint.x << ", " << collisionPoint.y << ", " << collisionPoint.z << ")\n";
                    std::cout << "  Impact speed: " << collisionSpeed << " m/s\n";
                    std::cout << "  Bodies: Mass " << bodies[i]->mass << " vs Mass " << bodies[j]->mass << "\n";
                }
            }
        }
    }
}
