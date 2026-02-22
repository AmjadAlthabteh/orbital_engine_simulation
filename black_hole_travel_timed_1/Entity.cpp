#include "Entity.hpp"

Entity::Entity()
{
    position = Vec3(0.0f, 0.0f, 0.0f);
    scale = Vec3(1.0f, 1.0f, 1.0f);
}

void Entity::setPosition(const Vec3& pos)
{
    position = pos;
}

void Entity::setScale(const Vec3& scl)
{
    scale = scl;
}

Vec3 Entity::getPosition() const
{
    return position;
}

Mat4 Entity::getModelMatrix() const
{
    Mat4 T = Mat4::translation(position.x, position.y, position.z);
    Mat4 S = Mat4::scale(scale.x);

    return T * S;
}