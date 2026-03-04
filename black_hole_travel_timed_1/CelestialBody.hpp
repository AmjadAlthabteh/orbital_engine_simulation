#pragma once

#include "Entity.hpp"
#include "Body.hpp"
#include "Trail.hpp"
#include <string>
#include <memory>

class CelestialBody : public Entity
{
private:
    std::string name;
    float radius;
    Body physicsBody;
    Vec3 color;

    float rotationAngle;
    float rotationSpeed;
    Vec3 rotationAxis;

    std::unique_ptr<Trail> trail;
    bool hasTrail;
    float trailTimer;
    float trailInterval;

public:
    CelestialBody(const std::string& name,
        float mass,
        float radius,
        const Vec3& color = Vec3(1.0f, 1.0f, 1.0f));

    void update(float deltaTime) override;
    void enableTrail(size_t maxPoints = 500);
    void renderTrail();

    Body& getPhysicsBody();

    const std::string& getName() const;
    float getRadius() const;
    const Vec3& getColor() const;
    float getRotationAngle() const;
    const Vec3& getRotationAxis() const;
};