#pragma once
#include "Vec3.hpp"
#include <vector>

class Body
{
private:
    Vec3 forceAccum;

public:
    Vec3 position;
    Vec3 velocity;
    Vec3 acceleration;

    float mass;
    float inverseMass;

    float radius;

    bool isStatic;

    std::vector<Vec3> trailHistory;

    Body(float m = 1.0f, float r = 1.0f);

    void applyForce(const Vec3& force);

    void applyImpulse(const Vec3& impulse);

    void integrate(float deltaTime);

    void clearForces();

    float getSpeed() const;

    float kineticEnergy() const;
};