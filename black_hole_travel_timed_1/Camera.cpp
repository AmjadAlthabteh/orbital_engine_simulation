#include "Camera.hpp"
#include "Spaceship.hpp"
#include <cmath>

#define DEG2RAD 0.01745329251f

Camera::Camera()
{
    position = Vec3(0.0f, 5.0f, 30.0f);
    worldUp = Vec3(0.0f, 1.0f, 0.0f);

    yaw = -90.0f;
    pitch = 0.0f;

    mode = CameraMode::FREE;  // Start in free camera mode

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

void Camera::toggleMode()
{
    if (mode == CameraMode::FREE)
        mode = CameraMode::FOLLOW_SHIP;
    else if (mode == CameraMode::FOLLOW_SHIP)
        mode = CameraMode::CHASE_SHIP;
    else
        mode = CameraMode::FREE;
}

CameraMode Camera::getMode() const
{
    return mode;
}

const char* Camera::getModeName() const
{
    switch (mode)
    {
        case CameraMode::FREE: return "Free Camera";
        case CameraMode::FOLLOW_SHIP: return "Follow Ship";
        case CameraMode::CHASE_SHIP: return "Chase Ship";
        default: return "Unknown";
    }
}

void Camera::updateFollowMode(const Spaceship& ship)
{
    // Position camera behind and above the ship
    Vec3 shipPos = ship.getPhysicsBody().position;
    Vec3 offset(0, 5, -15);  // Behind and above

    position = shipPos + offset;

    // Look at the ship
    Vec3 lookTarget = shipPos;
    Vec3 direction = (lookTarget - position).normalize();

    // Calculate yaw and pitch from direction
    yaw = std::atan2(direction.z, direction.x) * (180.0f / 3.14159265f);
    pitch = std::asin(direction.y) * (180.0f / 3.14159265f);

    updateVectors();
}

void Camera::updateChaseMode(const Spaceship& ship)
{
    // Position camera behind the ship based on ship's orientation
    Vec3 shipPos = ship.getPhysicsBody().position;
    Vec3 shipForward = ship.getForward();
    Vec3 shipUp = ship.getUp();

    // Camera offset: behind the ship
    float distanceBehind = 20.0f;
    float heightAbove = 5.0f;

    position = shipPos - shipForward * distanceBehind + shipUp * heightAbove;

    // Look in the direction the ship is facing
    front = shipForward;

    // Match ship's orientation
    right = ship.getRight();
    up = shipUp;

    // Update yaw/pitch to match (for consistency)
    yaw = ship.getYaw();
    pitch = ship.getPitch();
}