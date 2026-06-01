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

    fov = 45.0f;          // Default field of view
    targetFov = 45.0f;    // Default target FOV
    zoomSpeed = 8.0f;     // Zoom interpolation speed - increased for more responsive zoom

    // Camera shake initialization
    shakeIntensity = 0.0f;
    shakeDecay = 5.0f;    // Shake fades quickly
    shakeOffset = Vec3(0, 0, 0);

    // Speed-based FOV initialization
    baseFov = 45.0f;              // Normal FOV
    maxSpeedFov = 75.0f;          // Wide FOV at high speeds
    speedFovTransition = 2.0f;    // Smooth transition speed

    // Cinematic camera roll initialization
    roll = 0.0f;                  // Start level
    targetRoll = 0.0f;
    rollSpeed = 8.0f;             // Fast but smooth response
    maxRollAngle = 15.0f;         // 15 degrees max tilt (like racing games)

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
    // Apply camera shake offset for extra juice!
    Vec3 shakenPosition = position + shakeOffset;

    Vec3 zaxis = (shakenPosition - (shakenPosition + front)).normalize();
    Vec3 xaxis = worldUp.cross(zaxis).normalize();
    Vec3 yaxis = zaxis.cross(xaxis);

    // CINEMATIC ROLL: Apply camera roll for that racing game feel!
    if (std::abs(roll) > 0.01f)
    {
        float rollRadians = roll * DEG2RAD;
        float cosRoll = std::cos(rollRadians);
        float sinRoll = std::sin(rollRadians);

        // Rotate the up and right vectors around the front (z) axis
        Vec3 newXaxis = xaxis * cosRoll + yaxis * sinRoll;
        Vec3 newYaxis = yaxis * cosRoll - xaxis * sinRoll;

        xaxis = newXaxis;
        yaxis = newYaxis;
    }

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

    view.m[12] = -xaxis.dot(shakenPosition);
    view.m[13] = -yaxis.dot(shakenPosition);
    view.m[14] = -zaxis.dot(shakenPosition);

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

void Camera::zoom(float amount)
{
    targetFov -= amount;

    // Clamp FOV to reasonable range (10 to 120 degrees)
    if (targetFov < 10.0f) targetFov = 10.0f;
    if (targetFov > 120.0f) targetFov = 120.0f;
}

void Camera::setZoom(float newFov)
{
    targetFov = newFov;

    // Clamp FOV
    if (targetFov < 10.0f) targetFov = 10.0f;
    if (targetFov > 120.0f) targetFov = 120.0f;
}

float Camera::getFOV() const
{
    return fov;
}

void Camera::updateZoom(float deltaTime)
{
    // Smoothly interpolate FOV toward target
    if (std::abs(fov - targetFov) > 0.1f)
    {
        float diff = targetFov - fov;
        fov += diff * zoomSpeed * deltaTime;
    }
    else
    {
        fov = targetFov;
    }
}

// OPTIMIZATION: Frustum culling - check if sphere is visible in camera view
bool Camera::isSphereInFrustum(const Vec3& center, float radius, float aspectRatio) const
{
    // PERFORMANCE: Early rejection using distance-squared (avoids sqrt)
    // Cull objects beyond far plane distance (assume 10000 units)
    constexpr float MAX_RENDER_DISTANCE_SQ = 10000.0f * 10000.0f;
    const Vec3 toCenter = center - position;
    const float distSq = toCenter.x * toCenter.x + toCenter.y * toCenter.y + toCenter.z * toCenter.z;

    if (distSq > MAX_RENDER_DISTANCE_SQ)
        return false;

    // Check if behind camera (with radius tolerance)
    const float distanceAlongFront = toCenter.dot(front);
    if (distanceAlongFront < -radius)
        return false;

    // Calculate half-angles for frustum (cache to avoid recomputation)
    const float fovRadians = fov * DEG2RAD;
    const float halfVFov = fovRadians * 0.5f;
    const float halfHFov = std::atan(std::tan(halfVFov) * aspectRatio);

    // OPTIMIZATION: Check vertical bounds first (fails more often)
    const float distanceAlongUp = toCenter.dot(up);
    const float verticalExtent = distanceAlongFront * std::tan(halfVFov) + radius;
    if (std::abs(distanceAlongUp) > verticalExtent)
        return false;

    // Check horizontal frustum bounds
    const float distanceAlongRight = toCenter.dot(right);
    const float horizontalExtent = distanceAlongFront * std::tan(halfHFov) + radius;
    if (std::abs(distanceAlongRight) > horizontalExtent)
        return false;

    return true;
}

// ============================================
// CAMERA SHAKE IMPLEMENTATION - MAKE IT JUICY!
// ============================================

void Camera::addShake(float intensity)
{
    shakeIntensity += intensity;
    // Cap max shake intensity
    if (shakeIntensity > 2.0f) shakeIntensity = 2.0f;
}

void Camera::updateShake(float deltaTime)
{
    if (shakeIntensity > 0.01f)
    {
        // Generate random shake offset
        float randX = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
        float randY = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;
        float randZ = ((float)rand() / RAND_MAX - 0.5f) * 2.0f;

        shakeOffset = Vec3(randX, randY, randZ) * shakeIntensity;

        // Decay shake over time
        shakeIntensity -= shakeDecay * deltaTime;
        if (shakeIntensity < 0.0f) shakeIntensity = 0.0f;
    }
    else
    {
        shakeOffset = Vec3(0, 0, 0);
        shakeIntensity = 0.0f;
    }
}

Vec3 Camera::getShakeOffset() const
{
    return shakeOffset;
}

// ============================================
// SPEED-BASED FOV - Dynamic field of view!
// ============================================

void Camera::updateSpeedFOV(float currentSpeed, float deltaTime)
{
    // Map speed to FOV (0 to 200 speed -> baseFov to maxSpeedFov)
    float speedNormalized = std::min(currentSpeed / 200.0f, 1.0f);  // Clamp to 0-1
    float desiredFov = baseFov + (maxSpeedFov - baseFov) * speedNormalized;

    // Smooth FOV transition
    float fovDiff = desiredFov - targetFov;
    targetFov += fovDiff * speedFovTransition * deltaTime;

    // Extra FOV boost during hyperdrive!
    if (currentSpeed > 400.0f)
    {
        targetFov = std::min(targetFov + 10.0f, 90.0f);  // Extra wide FOV for warp!
    }
}

// ============================================
// CINEMATIC CAMERA ROLL - Banking on turns!
// ============================================

void Camera::updateCinematicRoll(float rotationInput, float deltaTime)
{
    // rotationInput: positive = rotating right, negative = rotating left
    // We want to tilt INTO the turn (right turn = right tilt)

    // Calculate target roll based on rotation input
    targetRoll = rotationInput * maxRollAngle;

    // Clamp target roll
    if (targetRoll > maxRollAngle) targetRoll = maxRollAngle;
    if (targetRoll < -maxRollAngle) targetRoll = -maxRollAngle;

    // Smoothly interpolate current roll toward target
    float rollDiff = targetRoll - roll;
    roll += rollDiff * rollSpeed * deltaTime;

    // Snap to zero if very close (prevent jitter)
    if (std::abs(roll) < 0.1f && std::abs(targetRoll) < 0.1f)
    {
        roll = 0.0f;
    }
}