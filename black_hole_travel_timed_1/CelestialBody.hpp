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

public:
    CelestialBody(const std::string& name,
        float mass,
        float radius);

    void update(float deltaTime) override;

    Body& getPhysicsBody();

    const std::string& getName() const;
    float getRadius() const;
};