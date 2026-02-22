// mass velocity position, accelerations , radius and trail history
#pragma once
#pragma once
#include "Vec3.hpp"

class Body
{
private:
    Vec3 forceAccum;

public:
    Vec3 position;
    Vec3 velocity;
    float mass;

    Body(float m = 1.0f);

    void applyForce(const Vec3& force);
    void integrate(float deltaTime);
    void clearForces();
};