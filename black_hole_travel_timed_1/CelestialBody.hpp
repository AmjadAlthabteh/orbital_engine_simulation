#pragma once
#include "Vec3.hpp"

class CelestialBody
{
public:
    Vec3 position;
    Vec3 velocity;
    Vec3 acceleration;

    float radius;
    float mass;

    Vec3 color;

    CelestialBody(
        const Vec3& pos,
        const Vec3& vel,
        float r,
        float m,
        const Vec3& col
    );

    void resetAcceleration();
};