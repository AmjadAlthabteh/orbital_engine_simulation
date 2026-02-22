#include "CelestialBody.hpp"

CelestialBody::CelestialBody(
    const Vec3& pos,
    const Vec3& vel,
    float r,
    float m,
    const Vec3& col)
    : position(pos),
    velocity(vel),
    radius(r),
    mass(m),
    color(col)
{
    acceleration = { 0,0,0 };
}

void CelestialBody::resetAcceleration()
{
    acceleration = { 0,0,0 };
}