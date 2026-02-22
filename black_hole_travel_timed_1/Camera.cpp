#include "Camera.hpp"
#include <cmath>

#define DEG2RAD 0.01745329251f

Camera::Camera()
{
    position = Vec3(0.0f, 5.0f, 30.0f);
    worldUp = Vec3(0.0f, 1.0f, 0.0f);

    yaw = -90.0f;
    pitch = 0.0f;

    updateVectors();
}

void Camera::updateVectors()
{
    Vec3 direction;
    direction.x = cos(yaw * DEG2RAD) * cos(pitch * DEG2RAD);
    direction.y = sin(pitch * DEG2RAD);
    direction.z = sin(yaw * DEG2RAD) * cos(pitch * DEG2RAD);

    front = direction.normalize();
    right = front.cross(worldUp).normalize();
    up = right.cross(front).normalize();
}

Mat4 Camera::getViewMatrix() const
{
    Vec3 zaxis = (position - (position + front)).normalize();
    Vec3 xaxis = worldUp.cross(zaxis).normalize();
    Vec3 yaxis = zaxis.cross(xaxis);

    Mat4 view = Mat4::identity();

    view.m[0] = xaxis.x;
    view.m[1] = yaxis.x;
    view.m[2] = zaxis.x;

    view.m[4] = xaxis.y;
    view.m[5] = yaxis.y;
    view.m[6] = zaxis.y;

    view.m[8] = xaxis.z;
    view.m[9] = yaxis.z;
    view.m[10] = zaxis.z;

    view.m[12] = -xaxis.dot(position);
    view.m[13] = -yaxis.dot(position);
    view.m[14] = -zaxis.dot(position);

    return view;
}

Vec3 Camera::getPosition() const
{
    return position;
}

void Camera::setPosition(const Vec3& pos)
{
    position = pos;
}