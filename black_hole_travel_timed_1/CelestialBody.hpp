#pragma once

#include "Entity.hpp"
#include "Body.hpp"
#include <string>

class CelestialBody : public Entity
{
private:
    std::string name;
    float radius;
    Body physicsBody;
    Vec3 color;

public:
    CelestialBody(const std::string& name,
        float mass,
        float radius,
        const Vec3& color = Vec3(1.0f, 1.0f, 1.0f));

    void update(float deltaTime) override;

    Body& getPhysicsBody();

    const std::string& getName() const;
    float getRadius() const;
    const Vec3& getColor() const;
};