#include "CollisionPredictor.hpp"
#include <cmath>
#include <algorithm>

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
    float distance = r.length();

    if (distance < 0.01f)
        return body.position;

    float forceMagnitude = G * body.mass * otherBody.mass / (distance * distance);
    Vec3 force = r.normalize() * forceMagnitude;

    Vec3 acceleration = force * (1.0f / body.mass);
    Vec3 futureVelocity = body.velocity + acceleration * time;
    Vec3 futurePosition = body.position + body.velocity * time + acceleration * (0.5f * time * time);

    return futurePosition;
}

void CollisionPredictor::predictCollisions(std::vector<Body*>& bodies, float G, float maxTime)
{
    predictions.clear();
    lineVertices.clear();

    for (size_t i = 0; i < bodies.size(); ++i)
    {
        for (size_t j = i + 1; j < bodies.size(); ++j)
        {
            Vec3 relativePos = bodies[j]->position - bodies[i]->position;
            Vec3 relativeVel = bodies[j]->velocity - bodies[i]->velocity;

            float minDist = bodies[i]->radius + bodies[j]->radius;

            float a = relativeVel.dot(relativeVel);
            float b = 2.0f * relativePos.dot(relativeVel);
            float c = relativePos.dot(relativePos) - minDist * minDist;

            float discriminant = b * b - 4.0f * a * c;

            if (discriminant >= 0.0f && a != 0.0f)
            {
                float t1 = (-b - std::sqrt(discriminant)) / (2.0f * a);
                float t2 = (-b + std::sqrt(discriminant)) / (2.0f * a);

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
    // CRITICAL SAFETY: Multiple checks before buffer access
    if (lineVertices.empty() || lineVertices.size() == 0)
        return;

    // EXTRA SAFETY: Verify reasonable size
    if (lineVertices.size() > 100000)  // Sanity check
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

    // Create temporary simulation of the body
    Vec3 simPos = body.position;
    Vec3 simVel = body.velocity;

    outPoints.push_back(simPos);

    for (int i = 0; i < numPoints; ++i)
    {
        // Calculate gravitational forces from all other bodies
        Vec3 totalForce(0, 0, 0);

        for (Body* other : otherBodies)
        {
            Vec3 direction = other->position - simPos;
            float distance = direction.length();

            if (distance < 0.1f)
                continue;

            float forceMagnitude = G * body.mass * other->mass / (distance * distance);
            Vec3 force = direction.normalize() * forceMagnitude;
            totalForce = totalForce + force;
        }

        // Update position and velocity using Euler integration
        Vec3 acceleration = totalForce * (1.0f / body.mass);
        simVel = simVel + acceleration * timeStep;
        simPos = simPos + simVel * timeStep;

        outPoints.push_back(simPos);
    }
}
