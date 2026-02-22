#pragma once
#include "Vec3.hpp"
#include "Mat4.hpp"

class Camera
{
public:
    Vec3 position;
    Vec3 front;
    Vec3 up;
    Vec3 right;
    Vec3 worldUp;

    float yaw;
    float pitch;

    Camera();

    Mat4 getViewMatrix() const;
    void updateVectors();

    Vec3 getPosition() const;
    void setPosition(const Vec3& pos);
};