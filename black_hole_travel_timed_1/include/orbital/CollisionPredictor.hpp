#pragma once
#include "Vec3.hpp"
#include "Body.hpp"
#include <vector>
#include <glad/glad.h>

struct CollisionPrediction
{
    Body* bodyA;
    Body* bodyB;
    Vec3 collisionPoint;
    float timeToCollision;
    bool willCollide;
};

class CollisionPredictor
{
private:
    std::vector<CollisionPrediction> predictions;
    unsigned int VAO, VBO;
    std::vector<Vec3> lineVertices;

public:
    CollisionPredictor();
    ~CollisionPredictor();

    void predictCollisions(std::vector<Body*>& bodies, float G, float maxTime = 100.0f);
    void calculateTrajectoryPoints(const Body& body, std::vector<Body*>& otherBodies, float G, float timeStep, int numPoints, std::vector<Vec3>& outPoints);
    void render();
    const std::vector<CollisionPrediction>& getPredictions() const;

private:
    void setupBuffers();
    void updateBuffers();
    Vec3 calculateFuturePosition(const Body& body, const Body& otherBody, float G, float time);
};
