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
    float fov;            // Field of view for zoom
    float targetFov;      // Target FOV for smooth zoom
    float zoomSpeed;      // Zoom interpolation speed

    // CAMERA SHAKE - Makes things feel INTENSE!
    float shakeIntensity;     // Current shake strength
    float shakeDecay;         // How fast shake fades (per second)
    Vec3 shakeOffset;         // Random offset for shake effect

    // SPEED-BASED FOV - Feel the speed!
    float baseFov;            // Normal FOV when stationary
    float maxSpeedFov;        // Max FOV at high speeds (60 degrees)
    float speedFovTransition; // How quickly FOV changes with speed

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

    // Zoom functionality
    void zoom(float amount);
    void setZoom(float newFov);
    float getFOV() const;
    void updateZoom(float deltaTime);  // Smooth zoom interpolation

    // OPTIMIZATION: Frustum culling - check if sphere is visible
    bool isSphereInFrustum(const Vec3& center, float radius, float aspectRatio) const;

    // CAMERA SHAKE - Add some juice to the game!
    void addShake(float intensity);         // Add shake effect
    void updateShake(float deltaTime);      // Update shake animation
    Vec3 getShakeOffset() const;            // Get current shake offset

    // SPEED-BASED FOV - Dynamic field of view
    void updateSpeedFOV(float currentSpeed, float deltaTime);  // Adjust FOV based on speed
    void setBaseFOV(float fov) { baseFov = fov; }              // Set normal FOV
};