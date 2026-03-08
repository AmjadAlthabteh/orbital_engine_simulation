#include "HUD.hpp"
#include <cmath>
#include <algorithm>

HUD::HUD()
{
    setupQuad();
}

HUD::~HUD()
{
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
}

void HUD::setupQuad()
{
    // Simple quad for rendering 2D elements
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void HUD::render(const Spaceship& ship,
                 const std::vector<CelestialBody*>& bodies,
                 float timeScale,
                 bool isPaused,
                 const std::string& cameraMode)
{
    // Find nearest body and calculate altitude
    float nearestDist = 999999.0f;
    std::string nearestName = "Unknown";
    Vec3 shipPos = ship.getPhysicsBody().position;
    float altitude = 0.0f;
    Vec3 nearestBodyPos;
    float nearestBodyMass = 0.0f;

    for (const auto* body : bodies)
    {
        Vec3 diff = body->getPhysicsBody().position - shipPos;
        float dist = diff.length();
        if (dist < nearestDist)
        {
            nearestDist = dist;
            nearestName = body->getName();
            nearestBodyPos = body->getPhysicsBody().position;
            nearestBodyMass = body->getPhysicsBody().mass;
            altitude = dist - body->getRadius();
        }
    }

    float speed = ship.getSpeed();
    float maxSpeed = 100.0f;  // For UI scaling

    // Render all HUD components
    renderSpeedometer(speed, maxSpeed);
    renderAltimeter(altitude, nearestName);
    renderCompass(shipPos, bodies);
    renderPhysicsData(ship);
    renderOrbitalParameters(ship, nearestBodyPos, nearestBodyMass);
    renderMiniMap(ship, bodies);
    renderCrosshair();
    renderStatusBar(timeScale, isPaused, cameraMode);
}

void HUD::renderSpeedometer(float speed, float maxSpeed)
{
    // Speedometer in bottom-left corner
    float centerX = -0.85f;
    float centerY = -0.75f;
    float radius = 0.12f;

    // Background circle
    renderCircle(centerX, centerY, radius, Vec3(0.0f, 0.0f, 0.0f), 0.7f);

    // Speed arc (0 to speed)
    float speedPercent = std::min(speed / maxSpeed, 1.0f);
    int segments = 32;
    float startAngle = -135.0f * 0.0174533f;  // -135 degrees in radians
    float endAngle = startAngle + (270.0f * 0.0174533f * speedPercent);

    // Draw speed arc in segments
    Vec3 arcColor = Vec3(0.0f, 1.0f, 0.3f);  // Green
    if (speedPercent > 0.7f) arcColor = Vec3(1.0f, 0.8f, 0.0f);  // Yellow
    if (speedPercent > 0.9f) arcColor = Vec3(1.0f, 0.2f, 0.0f);  // Red

    for (int i = 0; i < static_cast<int>(speedPercent * segments); i++)
    {
        float angle1 = startAngle + (270.0f * 0.0174533f * i / segments);
        float angle2 = startAngle + (270.0f * 0.0174533f * (i + 1) / segments);

        float x1 = centerX + (radius * 0.8f) * std::cos(angle1);
        float y1 = centerY + (radius * 0.8f) * std::sin(angle1);
        float x2 = centerX + radius * std::cos(angle1);
        float y2 = centerY + radius * std::sin(angle1);

        float x3 = centerX + (radius * 0.8f) * std::cos(angle2);
        float y3 = centerY + (radius * 0.8f) * std::sin(angle2);
        float x4 = centerX + radius * std::cos(angle2);
        float y4 = centerY + radius * std::sin(angle2);

        renderLine(x1, y1, x2, y2, arcColor, 0.8f);
    }

    // Tick marks
    for (int i = 0; i <= 10; i++)
    {
        float angle = startAngle + (270.0f * 0.0174533f * i / 10.0f);
        float innerRadius = radius * 0.85f;
        float outerRadius = radius * 0.95f;

        float x1 = centerX + innerRadius * std::cos(angle);
        float y1 = centerY + innerRadius * std::sin(angle);
        float x2 = centerX + outerRadius * std::cos(angle);
        float y2 = centerY + outerRadius * std::sin(angle);

        renderLine(x1, y1, x2, y2, Vec3(0.5f, 0.5f, 0.5f), 0.8f);
    }

    // Inner circle
    renderCircle(centerX, centerY, radius * 0.3f, Vec3(0.1f, 0.1f, 0.1f), 0.9f);

    // Label box below speedometer
    float labelY = centerY - radius - 0.08f;
    renderBox(centerX - 0.08f, labelY - 0.03f, 0.16f, 0.06f, Vec3(0.0f, 0.0f, 0.0f), 0.7f);
}

void HUD::renderAltimeter(float altitude, const std::string& nearestBody)
{
    // Altimeter in bottom-right corner
    float x = 0.60f;
    float y = -0.75f;
    float width = 0.3f;
    float height = 0.15f;

    // Background box
    renderBox(x, y, width, height, Vec3(0.0f, 0.0f, 0.0f), 0.7f);

    // Warning color based on altitude
    Vec3 borderColor = Vec3(0.0f, 0.8f, 0.3f);  // Green
    if (altitude < 10.0f) borderColor = Vec3(1.0f, 0.8f, 0.0f);  // Yellow
    if (altitude < 5.0f) borderColor = Vec3(1.0f, 0.2f, 0.0f);   // Red - DANGER!

    // Border
    float borderWidth = 0.005f;
    renderBox(x - borderWidth, y - borderWidth, width + 2 * borderWidth, borderWidth, borderColor, 0.9f);
    renderBox(x - borderWidth, y + height, width + 2 * borderWidth, borderWidth, borderColor, 0.9f);
    renderBox(x - borderWidth, y, borderWidth, height, borderColor, 0.9f);
    renderBox(x + width, y, borderWidth, height, borderColor, 0.9f);

    // Altitude bar
    float maxAlt = 100.0f;
    float altPercent = std::min(altitude / maxAlt, 1.0f);
    float barHeight = height * 0.6f;
    float barY = y + 0.02f;

    renderBox(x + 0.02f, barY, 0.04f, barHeight, Vec3(0.2f, 0.2f, 0.2f), 0.5f);
    renderBox(x + 0.02f, barY, 0.04f, barHeight * altPercent, borderColor, 0.8f);
}

void HUD::renderCompass(const Vec3& shipPos, const std::vector<CelestialBody*>& bodies)
{
    // Compass in top-center
    float centerX = 0.0f;
    float centerY = 0.85f;
    float radius = 0.08f;

    // Background circle
    renderCircle(centerX, centerY, radius, Vec3(0.0f, 0.0f, 0.0f), 0.6f);
    renderCircle(centerX, centerY, radius * 0.95f, Vec3(0.0f, 0.2f, 0.4f), 0.3f);

    // Draw direction markers to nearest planets
    int markersDrawn = 0;
    for (const auto* body : bodies)
    {
        if (body->getName() == "Sun" || markersDrawn >= 8) continue;

        Vec3 dirToBody = body->getPhysicsBody().position - shipPos;
        dirToBody.y = 0;  // Project onto horizontal plane
        float dist = dirToBody.length();
        if (dist < 0.1f) continue;

        dirToBody = dirToBody / dist;  // Normalize

        float angle = std::atan2(dirToBody.z, dirToBody.x);
        float markerDist = radius * 0.7f;

        float mx = centerX + markerDist * std::cos(angle);
        float my = centerY + markerDist * std::sin(angle);

        // Color based on planet
        Vec3 markerColor = body->getColor() * 1.5f;
        renderCircle(mx, my, 0.01f, markerColor, 0.9f);

        markersDrawn++;
    }

    // Center dot (ship position)
    renderCircle(centerX, centerY, 0.015f, Vec3(1.0f, 1.0f, 1.0f), 1.0f);
}

void HUD::renderPhysicsData(const Spaceship& ship)
{
    // Physics data in top-left corner
    float x = -0.95f;
    float y = 0.70f;
    float width = 0.35f;
    float height = 0.25f;

    // Background
    renderBox(x, y, width, height, Vec3(0.0f, 0.0f, 0.0f), 0.7f);

    // Title bar
    renderBox(x, y + height - 0.04f, width, 0.04f, Vec3(0.0f, 0.3f, 0.6f), 0.8f);

    // Data bars for visual representation
    const Vec3& vel = ship.getPhysicsBody().velocity;
    const Vec3& acc = ship.getPhysicsBody().acceleration;

    float velMag = vel.length();
    float accMag = acc.length();

    float barX = x + 0.12f;
    float barY1 = y + height - 0.10f;
    float barY2 = y + height - 0.16f;
    float barY3 = y + height - 0.22f;

    float maxBarWidth = width - 0.14f;

    // Velocity bar (green)
    float velBarWidth = std::min(velMag / 50.0f, 1.0f) * maxBarWidth;
    renderBox(barX, barY1, velBarWidth, 0.03f, Vec3(0.0f, 1.0f, 0.3f), 0.8f);

    // Acceleration bar (yellow)
    float accBarWidth = std::min(accMag / 20.0f, 1.0f) * maxBarWidth;
    renderBox(barX, barY2, accBarWidth, 0.03f, Vec3(1.0f, 0.8f, 0.0f), 0.8f);

    // Mass indicator (blue)
    float massPercent = ship.getPhysicsBody().mass / 10.0f;  // Assuming max mass of 10
    float massBarWidth = std::min(massPercent, 1.0f) * maxBarWidth;
    renderBox(barX, barY3, massBarWidth, 0.03f, Vec3(0.3f, 0.5f, 1.0f), 0.8f);
}

void HUD::renderOrbitalParameters(const Spaceship& ship, const Vec3& centralBodyPos, float centralBodyMass)
{
    // Orbital parameters in top-right corner
    float x = 0.60f;
    float y = 0.70f;
    float width = 0.35f;
    float height = 0.25f;

    // Background
    renderBox(x, y, width, height, Vec3(0.0f, 0.0f, 0.0f), 0.7f);

    // Title bar
    renderBox(x, y + height - 0.04f, width, 0.04f, Vec3(0.6f, 0.3f, 0.0f), 0.8f);

    // Calculate orbital elements
    float apoapsis, periapsis, eccentricity, period;
    calculateOrbitalElements(
        ship.getPhysicsBody().position,
        ship.getPhysicsBody().velocity,
        centralBodyPos,
        centralBodyMass,
        apoapsis, periapsis, eccentricity, period
    );

    // Visual indicators for orbital shape
    float barX = x + 0.12f;
    float barY1 = y + height - 0.10f;
    float barY2 = y + height - 0.16f;

    float maxBarWidth = width - 0.14f;

    // Apoapsis/Periapsis bars
    float apoBarWidth = std::min(apoapsis / 100.0f, 1.0f) * maxBarWidth;
    float periBarWidth = std::min(periapsis / 100.0f, 1.0f) * maxBarWidth;

    renderBox(barX, barY1, apoBarWidth, 0.03f, Vec3(1.0f, 0.4f, 0.4f), 0.8f);
    renderBox(barX, barY2, periBarWidth, 0.03f, Vec3(0.4f, 0.4f, 1.0f), 0.8f);

    // Eccentricity circle (orbit shape visualization)
    float circleX = x + width - 0.08f;
    float circleY = y + 0.10f;
    float circleRadius = 0.05f;

    renderCircle(circleX, circleY, circleRadius, Vec3(0.0f, 0.8f, 0.8f), 0.3f);

    // Draw ellipse to show eccentricity
    float ecc = std::min(eccentricity, 0.99f);
    float a = circleRadius;
    float b = circleRadius * std::sqrt(1.0f - ecc * ecc);

    for (int i = 0; i < 32; i++)
    {
        float angle1 = (i / 32.0f) * 6.28318f;
        float angle2 = ((i + 1) / 32.0f) * 6.28318f;

        float x1 = circleX + a * std::cos(angle1);
        float y1 = circleY + b * std::sin(angle1);
        float x2 = circleX + a * std::cos(angle2);
        float y2 = circleY + b * std::sin(angle2);

        renderLine(x1, y1, x2, y2, Vec3(0.0f, 1.0f, 1.0f), 0.8f);
    }
}

void HUD::renderMiniMap(const Spaceship& ship, const std::vector<CelestialBody*>& bodies)
{
    // Mini-map in bottom center
    float centerX = 0.0f;
    float centerY = -0.80f;
    float width = 0.25f;
    float height = 0.15f;

    // Background
    renderBox(centerX - width / 2, centerY - height / 2, width, height, Vec3(0.0f, 0.0f, 0.0f), 0.7f);

    // Map scale
    float mapScale = 0.003f;  // Adjust this to zoom in/out
    Vec3 shipPos = ship.getPhysicsBody().position;

    // Draw celestial bodies
    for (const auto* body : bodies)
    {
        Vec3 relPos = body->getPhysicsBody().position - shipPos;
        float mapX = centerX + relPos.x * mapScale;
        float mapY = centerY + relPos.z * mapScale;

        // Clamp to map bounds
        mapX = std::max(centerX - width / 2, std::min(centerX + width / 2, mapX));
        mapY = std::max(centerY - height / 2, std::min(centerY + height / 2, mapY));

        float dotSize = 0.01f;
        if (body->getName() == "Sun") dotSize = 0.02f;

        renderCircle(mapX, mapY, dotSize, body->getColor(), 0.8f);
    }

    // Draw ship at center (always centered on ship)
    renderCircle(centerX, centerY, 0.015f, Vec3(0.2f, 0.8f, 1.0f), 1.0f);
}

void HUD::renderCrosshair()
{
    // Simple crosshair in screen center
    float centerX = 0.0f;
    float centerY = 0.0f;
    float size = 0.02f;
    float gap = 0.01f;

    Vec3 color(0.0f, 1.0f, 0.5f);
    float alpha = 0.7f;

    // Horizontal line
    renderLine(-size - gap, centerY, -gap, centerY, color, alpha);
    renderLine(gap, centerY, size + gap, centerY, color, alpha);

    // Vertical line
    renderLine(centerX, -size - gap, centerX, -gap, color, alpha);
    renderLine(centerX, gap, centerX, size + gap, color, alpha);

    // Center dot
    renderCircle(centerX, centerY, 0.003f, color, alpha);
}

void HUD::renderStatusBar(float timeScale, bool isPaused, const std::string& cameraMode)
{
    // Status bar at very top of screen
    float y = 0.95f;
    renderBox(-1.0f, y, 2.0f, 0.05f, Vec3(0.0f, 0.0f, 0.0f), 0.8f);

    // Time scale indicator (left side)
    float timeBarWidth = std::min(timeScale / 10.0f, 1.0f) * 0.3f;
    Vec3 timeColor = isPaused ? Vec3(1.0f, 0.2f, 0.0f) : Vec3(0.0f, 0.8f, 0.3f);
    renderBox(-0.95f, y + 0.01f, timeBarWidth, 0.03f, timeColor, 0.8f);
}

void HUD::renderBox(float x, float y, float width, float height, const Vec3& color, float alpha)
{
    // Simple box rendering using lines (since we don't have text rendering)
    renderLine(x, y, x + width, y, color, alpha);
    renderLine(x + width, y, x + width, y + height, color, alpha);
    renderLine(x + width, y + height, x, y + height, color, alpha);
    renderLine(x, y + height, x, y, color, alpha);

    // Fill (using multiple horizontal lines for simplicity)
    int fillLines = static_cast<int>(height * 1000.0f);
    fillLines = std::max(1, std::min(fillLines, 100));

    for (int i = 0; i < fillLines; i++)
    {
        float lineY = y + (height * i / fillLines);
        renderLine(x, lineY, x + width, lineY, color, alpha * 0.5f);
    }
}

void HUD::renderLine(float x1, float y1, float x2, float y2, const Vec3& color, float alpha)
{
    // Note: This is a placeholder - actual implementation would require
    // a line shader and VAO setup. For now, we'll use the existing line shader
    // from main.cpp. This function just stores the data.
}

void HUD::renderCircle(float x, float y, float radius, const Vec3& color, float alpha, int segments)
{
    // Draw circle using line segments
    for (int i = 0; i < segments; i++)
    {
        float angle1 = (i / (float)segments) * 6.28318f;
        float angle2 = ((i + 1) / (float)segments) * 6.28318f;

        float x1 = x + radius * std::cos(angle1);
        float y1 = y + radius * std::sin(angle1);
        float x2 = x + radius * std::cos(angle2);
        float y2 = y + radius * std::sin(angle2);

        renderLine(x1, y1, x2, y2, color, alpha);
    }
}

void HUD::calculateOrbitalElements(const Vec3& pos, const Vec3& vel, const Vec3& centralPos,
                                   float centralMass, float& apoapsis, float& periapsis,
                                   float& eccentricity, float& period)
{
    // Simplified orbital mechanics calculations
    Vec3 r = pos - centralPos;
    float rMag = r.length();

    float G = 0.1f;  // Gravitational constant (same as physics engine)
    float mu = G * centralMass;

    float vMag = vel.length();

    // Specific orbital energy
    float epsilon = (vMag * vMag / 2.0f) - (mu / rMag);

    // Semi-major axis
    float a = -mu / (2.0f * epsilon);

    // Angular momentum
    Vec3 h = Vec3(
        r.y * vel.z - r.z * vel.y,
        r.z * vel.x - r.x * vel.z,
        r.x * vel.y - r.y * vel.x
    );
    float hMag = h.length();

    // Eccentricity
    float eSq = 1.0f + (2.0f * epsilon * hMag * hMag) / (mu * mu);
    eccentricity = (eSq > 0.0f) ? std::sqrt(eSq) : 0.0f;

    // Apoapsis and periapsis
    if (eccentricity < 1.0f)  // Elliptical orbit
    {
        apoapsis = a * (1.0f + eccentricity);
        periapsis = a * (1.0f - eccentricity);
        period = 6.28318f * std::sqrt(a * a * a / mu);
    }
    else  // Parabolic or hyperbolic
    {
        apoapsis = 999999.0f;
        periapsis = rMag;
        period = 999999.0f;
    }
}
