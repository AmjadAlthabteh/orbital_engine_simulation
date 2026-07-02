#include "CollisionPredictor.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>

namespace
{
    constexpr float kMinMass = 0.0001f;
    constexpr float kMinDistanceSquared = 0.0001f;
    constexpr float kMinRelativeSpeedSquared = 0.000001f;
    constexpr std::size_t kMaxLineVertices = 100000;
}

CollisionPredictor::CollisionPredictor()
{
    setupBuffers();
}

CollisionPredictor::~CollisionPredictor()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void CollisionPredictor::setupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Vec3 CollisionPredictor::calculateFuturePosition(const Body& body, const Body& otherBody, float G, float time)
{
    Vec3 r = otherBody.position - body.position;
    float distanceSquared = r.lengthSquared();

    if (body.mass <= kMinMass || distanceSquared < kMinDistanceSquared)
        return body.position;

    const float invDistance = 1.0f / std::sqrt(distanceSquared);
    float forceMagnitude = G * body.mass * otherBody.mass * (invDistance * invDistance);
    Vec3 force = r * (invDistance * forceMagnitude);

    Vec3 acceleration = force * (1.0f / body.mass);
    Vec3 futurePosition = body.position + body.velocity * time + acceleration * (0.5f * time * time);

    return futurePosition;
}

void CollisionPredictor::predictCollisions(std::vector<Body*>& bodies, float G, float maxTime)
{
    predictions.clear();
    lineVertices.clear();
    if (bodies.size() < 2 || maxTime <= 0.0f)
    {
        updateBuffers();
        return;
    }

    predictions.reserve(bodies.size());
    lineVertices.reserve(bodies.size() * 4);

    for (size_t i = 0; i < bodies.size(); ++i)
    {
        if (bodies[i] == nullptr)
            continue;

        for (size_t j = i + 1; j < bodies.size(); ++j)
        {
            if (bodies[j] == nullptr)
                continue;

            Vec3 relativePos = bodies[j]->position - bodies[i]->position;
            Vec3 relativeVel = bodies[j]->velocity - bodies[i]->velocity;

            float minDist = bodies[i]->radius + bodies[j]->radius;

            float a = relativeVel.dot(relativeVel);
            float b = 2.0f * relativePos.dot(relativeVel);
            float c = relativePos.dot(relativePos) - minDist * minDist;

            float discriminant = b * b - 4.0f * a * c;

            if (discriminant >= 0.0f && a > kMinRelativeSpeedSquared)
            {
                const float sqrtDiscriminant = std::sqrt(discriminant);
                const float inverseDenominator = 0.5f / a;
                float t1 = (-b - sqrtDiscriminant) * inverseDenominator;
                float t2 = (-b + sqrtDiscriminant) * inverseDenominator;

                float collisionTime = -1.0f;
                if (t1 > 0.0f && t1 < maxTime)
                    collisionTime = t1;
                else if (t2 > 0.0f && t2 < maxTime)
                    collisionTime = t2;

                if (collisionTime > 0.0f)
                {
                    Vec3 collisionPos = bodies[i]->position + bodies[i]->velocity * collisionTime;

                    CollisionPrediction pred;
                    pred.bodyA = bodies[i];
                    pred.bodyB = bodies[j];
                    pred.collisionPoint = collisionPos;
                    pred.timeToCollision = collisionTime;
                    pred.willCollide = true;

                    predictions.push_back(pred);

                    lineVertices.push_back(bodies[i]->position);
                    lineVertices.push_back(collisionPos);
                    lineVertices.push_back(bodies[j]->position);
                    lineVertices.push_back(collisionPos);
                }
            }
        }
    }

    updateBuffers();
}

void CollisionPredictor::updateBuffers()
{
    if (lineVertices.empty())
        return;

    if (lineVertices.size() > kMaxLineVertices)
    {
        lineVertices.clear();
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        lineVertices.size() * 3 * sizeof(float),
        lineVertices.data(),  // Use .data() - safer than &[0]
        GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CollisionPredictor::render()
{
    if (lineVertices.empty())
        return;

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lineVertices.size()));
    glBindVertexArray(0);
}

const std::vector<CollisionPrediction>& CollisionPredictor::getPredictions() const
{
    return predictions;
}

void CollisionPredictor::calculateTrajectoryPoints(const Body& body, std::vector<Body*>& otherBodies, float G, float timeStep, int numPoints, std::vector<Vec3>& outPoints)
{
    outPoints.clear();
    const int pointCount = std::max(0, numPoints);
    outPoints.reserve(static_cast<size_t>(pointCount) + 1);

    // Create temporary simulation of the body
    Vec3 simPos = body.position;
    Vec3 simVel = body.velocity;

    outPoints.push_back(simPos);

    for (int i = 0; i < pointCount; ++i)
    {
        // Calculate gravitational forces from all other bodies
        Vec3 totalForce(0, 0, 0);

        for (Body* other : otherBodies)
        {
            if (other == nullptr)
                continue;

            Vec3 direction = other->position - simPos;
            float distanceSquared = direction.lengthSquared();

            if (distanceSquared < 0.01f) // 0.1^2
                continue;

            const float invDistance = 1.0f / std::sqrt(distanceSquared);
            float forceMagnitude = G * body.mass * other->mass * (invDistance * invDistance);
            Vec3 force = direction * (invDistance * forceMagnitude);
            totalForce = totalForce + force;
        }

        // Update position and velocity using Euler integration
        Vec3 acceleration = totalForce * (1.0f / body.mass);
        simVel = simVel + acceleration * timeStep;
        simPos = simPos + simVel * timeStep;

        outPoints.push_back(simPos);
    }
}
