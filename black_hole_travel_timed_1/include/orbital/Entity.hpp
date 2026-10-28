#pragma once
#include "Vec3.hpp"
#include "Mat4.hpp"

class Entity
{
protected:
    Vec3 position;
    Vec3 scale;

public:
    Entity();
    virtual ~Entity() = default;

    void setPosition(const Vec3& pos);
    void setScale(const Vec3& scl);

    Vec3 getPosition() const;

    Mat4 getModelMatrix() const;

    virtual void update(float deltaTime) = 0;
};