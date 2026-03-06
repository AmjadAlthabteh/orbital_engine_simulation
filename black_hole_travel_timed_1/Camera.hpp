#pragma once
#include "Vec3.hpp"
#include "Mat4.hpp"

// Forward declaration
class Spaceship;

enum class CameraMode
{
    FREE,           // Free camera controlled by WASD + mouse
    FOLLOW_SHIP,    // Camera follows ship from behind
    CHASE_SHIP      // Camera chases ship (immersive view)
};

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

    CameraMode mode;

    Camera();

    Mat4 getViewMatrix() const;
    void updateVectors();

    Vec3 getPosition() const;
    void setPosition(const Vec3& pos);

    // Camera mode management
    void toggleMode();
    CameraMode getMode() const;
    const char* getModeName() const;

    // Update camera based on mode
    void updateFollowMode(const Spaceship& ship);
    void updateChaseMode(const Spaceship& ship);
};