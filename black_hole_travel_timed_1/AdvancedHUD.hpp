#pragma once

#include "Vec3.hpp"
#include "Spaceship.hpp"
#include "CelestialBody.hpp"
#include <imgui.h>
#include <vector>
#include <cmath>

// Professional HUD overlay system
class AdvancedHUD
{
private:
    // HUD state
    bool showTargetingReticle;
    bool showSpeedVector;
    bool showDistanceRuler;
    bool showNavCompass;
    bool showProgradeRetrograde;
    bool showOrbitIndicators;

    // Colors
    ImU32 colorReticle = IM_COL32(0, 255, 255, 255);      // Cyan
    ImU32 colorPrograde = IM_COL32(0, 255, 0, 255);       // Green
    ImU32 colorRetrograde = IM_COL32(255, 100, 0, 255);   // Orange
    ImU32 colorDanger = IM_COL32(255, 50, 50, 255);       // Red
    ImU32 colorNormal = IM_COL32(100, 200, 255, 200);     // Blue
    ImU32 colorTarget = IM_COL32(255, 255, 0, 255);       // Yellow

public:
    AdvancedHUD()
        : showTargetingReticle(true), showSpeedVector(true),
          showDistanceRuler(true), showNavCompass(true),
          showProgradeRetrograde(true), showOrbitIndicators(true)
    {}

    // Main HUD render function
    void render(const Spaceship& ship, const std::vector<CelestialBody*>& bodies,
                CelestialBody* lockedTarget, bool targetLocked)
    {
        ImDrawList* drawList = ImGui::GetForegroundDrawList();
        ImVec2 screenCenter(640, 360);  // 1280x720 / 2
        ImVec2 screenSize(1280, 720);

        // Render all HUD elements
        if (showTargetingReticle)
            renderTargetingReticle(drawList, screenCenter, targetLocked);

        if (showSpeedVector)
            renderSpeedVector(drawList, screenCenter, ship);

        if (showDistanceRuler && lockedTarget != nullptr)
            renderDistanceRuler(drawList, screenCenter, ship, lockedTarget);

        if (showNavCompass)
            renderNavigationCompass(drawList, ImVec2(1180, 620), ship, bodies);

        if (showProgradeRetrograde)
            renderProgradeRetrograde(drawList, screenCenter, ship);

        if (showOrbitIndicators && lockedTarget != nullptr)
            renderOrbitIndicators(drawList, screenCenter, ship, lockedTarget);

        // Corner overlays
        renderSpeedIndicator(drawList, ImVec2(100, 650), ship);
        renderAltitudeIndicator(drawList, ImVec2(100, 100), ship, bodies);
        renderGForceIndicator(drawList, ImVec2(1180, 100), ship);
    }

    // Targeting reticle - professional crosshair
    void renderTargetingReticle(ImDrawList* drawList, ImVec2 center, bool locked)
    {
        float size = 40.0f;
        float thickness = locked ? 3.0f : 2.0f;
        ImU32 color = locked ? IM_COL32(255, 0, 0, 255) : colorReticle;

        // Animated pulsing when locked
        if (locked)
        {
            float pulse = 1.0f + 0.2f * std::sin(ImGui::GetTime() * 8.0f);
            size *= pulse;
        }

        // Cross lines (with gap in middle)
        float gap = 8.0f;

        // Horizontal
        drawList->AddLine(ImVec2(center.x - size, center.y),
                         ImVec2(center.x - gap, center.y), color, thickness);
        drawList->AddLine(ImVec2(center.x + gap, center.y),
                         ImVec2(center.x + size, center.y), color, thickness);

        // Vertical
        drawList->AddLine(ImVec2(center.x, center.y - size),
                         ImVec2(center.x, center.y - gap), color, thickness);
        drawList->AddLine(ImVec2(center.x, center.y + gap),
                         ImVec2(center.x, center.y + size), color, thickness);

        // Corner brackets
        float bracketSize = 15.0f;
        float offset = 25.0f;

        // Top-left
        drawList->AddLine(ImVec2(center.x - offset, center.y - offset),
                         ImVec2(center.x - offset - bracketSize, center.y - offset), color, thickness);
        drawList->AddLine(ImVec2(center.x - offset, center.y - offset),
                         ImVec2(center.x - offset, center.y - offset - bracketSize), color, thickness);

        // Top-right
        drawList->AddLine(ImVec2(center.x + offset, center.y - offset),
                         ImVec2(center.x + offset + bracketSize, center.y - offset), color, thickness);
        drawList->AddLine(ImVec2(center.x + offset, center.y - offset),
                         ImVec2(center.x + offset, center.y - offset - bracketSize), color, thickness);

        // Bottom-left
        drawList->AddLine(ImVec2(center.x - offset, center.y + offset),
                         ImVec2(center.x - offset - bracketSize, center.y + offset), color, thickness);
        drawList->AddLine(ImVec2(center.x - offset, center.y + offset),
                         ImVec2(center.x - offset, center.y + offset + bracketSize), color, thickness);

        // Bottom-right
        drawList->AddLine(ImVec2(center.x + offset, center.y + offset),
                         ImVec2(center.x + offset + bracketSize, center.y + offset), color, thickness);
        drawList->AddLine(ImVec2(center.x + offset, center.y + offset),
                         ImVec2(center.x + offset, center.y + offset + bracketSize), color, thickness);

        // Center dot
        drawList->AddCircleFilled(center, 2.0f, color);
    }

    // Speed vector indicator
    void renderSpeedVector(ImDrawList* drawList, ImVec2 center, const Spaceship& ship)
    {
        Vec3 vel = ship.getPhysicsBody().velocity;
        float speed = ship.getSpeed();

        if (speed < 0.1f) return;  // Don't show if stationary

        // Calculate screen position based on velocity direction
        // This is a simplified 2D projection
        float scale = 3.0f;
        ImVec2 velocityIndicator(
            center.x + vel.x * scale,
            center.y - vel.z * scale  // Invert Z for screen coords
        );

        // Draw line from center to velocity indicator
        drawList->AddLine(center, velocityIndicator, colorNormal, 2.0f);

        // Arrow head
        float arrowSize = 8.0f;
        ImVec2 direction(velocityIndicator.x - center.x, velocityIndicator.y - center.y);
        float len = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (len > 0.01f)
        {
            direction.x /= len;
            direction.y /= len;

            ImVec2 arrowPoint = velocityIndicator;
            ImVec2 arrowLeft(
                arrowPoint.x - direction.x * arrowSize - direction.y * arrowSize * 0.5f,
                arrowPoint.y - direction.y * arrowSize + direction.x * arrowSize * 0.5f
            );
            ImVec2 arrowRight(
                arrowPoint.x - direction.x * arrowSize + direction.y * arrowSize * 0.5f,
                arrowPoint.y - direction.y * arrowSize - direction.x * arrowSize * 0.5f
            );

            drawList->AddTriangleFilled(arrowPoint, arrowLeft, arrowRight, colorNormal);
        }

        // Speed label
        char speedText[32];
        snprintf(speedText, sizeof(speedText), "%.1f m/s", speed);
        drawList->AddText(ImVec2(velocityIndicator.x + 10, velocityIndicator.y - 10),
                         colorNormal, speedText);
    }

    // Distance ruler to target
    void renderDistanceRuler(ImDrawList* drawList, ImVec2 center,
                            const Spaceship& ship, CelestialBody* target)
    {
        Vec3 toTarget = target->getPhysicsBody().position - ship.getPhysicsBody().position;
        float distance = toTarget.length();

        // Draw ruler on right side of screen
        ImVec2 rulerStart(1200, 200);
        ImVec2 rulerEnd(1200, 500);
        float rulerLength = rulerEnd.y - rulerStart.y;

        // Background
        drawList->AddRectFilled(ImVec2(rulerStart.x - 20, rulerStart.y - 10),
                               ImVec2(rulerEnd.x + 60, rulerEnd.y + 10),
                               IM_COL32(0, 0, 0, 150));

        // Ruler line
        drawList->AddLine(rulerStart, rulerEnd, colorNormal, 3.0f);

        // Distance markers
        float maxDist = 200.0f;  // Max distance to show
        for (int i = 0; i <= 4; ++i)
        {
            float frac = i / 4.0f;
            ImVec2 markerPos(rulerStart.x, rulerStart.y + rulerLength * frac);

            drawList->AddLine(ImVec2(markerPos.x - 5, markerPos.y),
                            ImVec2(markerPos.x + 5, markerPos.y),
                            colorNormal, 2.0f);

            char distText[16];
            snprintf(distText, sizeof(distText), "%.0f", maxDist * (1.0f - frac));
            drawList->AddText(ImVec2(markerPos.x + 10, markerPos.y - 8),
                            colorNormal, distText);
        }

        // Current distance indicator
        float distFrac = std::min(distance / maxDist, 1.0f);
        ImVec2 currentPos(rulerStart.x, rulerStart.y + rulerLength * (1.0f - distFrac));

        drawList->AddCircleFilled(currentPos, 6.0f, colorTarget);
        drawList->AddCircle(currentPos, 6.0f, IM_COL32(255, 255, 255, 255), 12, 2.0f);

        // Distance label
        char label[64];
        snprintf(label, sizeof(label), "%s\n%.1f units", target->getName().c_str(), distance);
        drawList->AddText(ImVec2(rulerStart.x - 100, currentPos.y - 20), colorTarget, label);
    }

    // Navigation compass
    void renderNavigationCompass(ImDrawList* drawList, ImVec2 pos,
                                const Spaceship& ship, const std::vector<CelestialBody*>& bodies)
    {
        float radius = 80.0f;

        // Background circle
        drawList->AddCircleFilled(pos, radius, IM_COL32(0, 0, 0, 180));
        drawList->AddCircle(pos, radius, colorNormal, 64, 3.0f);

        // Cardinal directions
        const char* directions[] = {"N", "E", "S", "W"};
        float angles[] = {-90, 0, 90, 180};

        for (int i = 0; i < 4; ++i)
        {
            float angleRad = angles[i] * 3.14159f / 180.0f;
            ImVec2 dirPos(
                pos.x + std::cos(angleRad) * (radius - 15),
                pos.y + std::sin(angleRad) * (radius - 15)
            );
            drawList->AddText(ImVec2(dirPos.x - 5, dirPos.y - 8),
                            IM_COL32(200, 200, 200, 255), directions[i]);
        }

        // Ship direction indicator (pointing forward)
        Vec3 forward = ship.getForward();
        float shipAngle = std::atan2(forward.z, forward.x);
        ImVec2 shipDir(
            pos.x + std::cos(shipAngle) * 30,
            pos.y + std::sin(shipAngle) * 30
        );
        drawList->AddLine(pos, shipDir, IM_COL32(0, 255, 0, 255), 3.0f);
        drawList->AddCircleFilled(shipDir, 5.0f, IM_COL32(0, 255, 0, 255));

        // Center label
        drawList->AddText(ImVec2(pos.x - 20, pos.y - 8), colorNormal, "NAV");
    }

    // Prograde/Retrograde markers
    void renderProgradeRetrograde(ImDrawList* drawList, ImVec2 center, const Spaceship& ship)
    {
        Vec3 vel = ship.getPhysicsBody().velocity;
        float speed = ship.getSpeed();

        if (speed < 0.1f) return;

        // Prograde (direction of movement) - circle with dot
        float offset = 100.0f;
        ImVec2 progradePos(center.x + vel.x * 2.0f, center.y - vel.z * 2.0f);

        drawList->AddCircle(progradePos, 15.0f, colorPrograde, 12, 3.0f);
        drawList->AddCircleFilled(progradePos, 3.0f, colorPrograde);

        // Lines radiating out
        for (int i = 0; i < 4; ++i)
        {
            float angle = i * 90.0f * 3.14159f / 180.0f;
            ImVec2 lineEnd(
                progradePos.x + std::cos(angle) * 20.0f,
                progradePos.y + std::sin(angle) * 20.0f
            );
            drawList->AddLine(progradePos, lineEnd, colorPrograde, 2.0f);
        }

        // Retrograde (opposite direction) - circle with X
        ImVec2 retrogradePos(center.x - vel.x * 2.0f, center.y + vel.z * 2.0f);

        drawList->AddCircle(retrogradePos, 15.0f, colorRetrograde, 12, 3.0f);
        drawList->AddLine(ImVec2(retrogradePos.x - 8, retrogradePos.y - 8),
                         ImVec2(retrogradePos.x + 8, retrogradePos.y + 8),
                         colorRetrograde, 3.0f);
        drawList->AddLine(ImVec2(retrogradePos.x - 8, retrogradePos.y + 8),
                         ImVec2(retrogradePos.x + 8, retrogradePos.y - 8),
                         colorRetrograde, 3.0f);
    }

    // Orbit indicators (radial/normal markers)
    void renderOrbitIndicators(ImDrawList* drawList, ImVec2 center,
                              const Spaceship& ship, CelestialBody* target)
    {
        Vec3 toTarget = target->getPhysicsBody().position - ship.getPhysicsBody().position;
        float dist = toTarget.length();
        if (dist < 0.01f) return;

        // Radial direction (toward/away from target)
        Vec3 radialDir(toTarget.x / dist, toTarget.y / dist, toTarget.z / dist);

        // Radial markers on screen edges
        ImVec2 radialInPos(center.x + radialDir.x * 150, center.y - radialDir.z * 150);
        ImVec2 radialOutPos(center.x - radialDir.x * 150, center.y + radialDir.z * 150);

        // Radial in (toward target) - triangle
        drawList->AddCircle(radialInPos, 12.0f, IM_COL32(255, 255, 0, 255), 3, 2.0f);

        // Radial out (away from target) - inverted triangle
        drawList->AddCircle(radialOutPos, 12.0f, IM_COL32(0, 255, 255, 255), 3, 2.0f);
    }

    // Speed indicator (bottom-left corner)
    void renderSpeedIndicator(ImDrawList* drawList, ImVec2 pos, const Spaceship& ship)
    {
        float speed = ship.getSpeed();
        float maxSpeed = 150.0f;  // Reference max

        // Background
        drawList->AddRectFilled(ImVec2(pos.x - 10, pos.y - 60),
                               ImVec2(pos.x + 150, pos.y + 10),
                               IM_COL32(0, 0, 0, 180));

        // Label
        drawList->AddText(ImVec2(pos.x, pos.y - 55), colorNormal, "SPEED");

        // Speed value
        char speedText[32];
        snprintf(speedText, sizeof(speedText), "%.1f m/s", speed);

        ImU32 speedColor = speed > 100.0f ? colorDanger : colorNormal;
        drawList->AddText(ImVec2(pos.x, pos.y - 35), speedColor, speedText);

        // Speed bar
        float barWidth = 130.0f;
        float speedFrac = std::min(speed / maxSpeed, 1.0f);

        drawList->AddRect(ImVec2(pos.x, pos.y - 20),
                         ImVec2(pos.x + barWidth, pos.y - 5),
                         colorNormal, 0.0f, 0, 2.0f);

        drawList->AddRectFilled(ImVec2(pos.x + 2, pos.y - 18),
                               ImVec2(pos.x + 2 + (barWidth - 4) * speedFrac, pos.y - 7),
                               speedColor);
    }

    // Altitude indicator (top-left corner)
    void renderAltitudeIndicator(ImDrawList* drawList, ImVec2 pos,
                                 const Spaceship& ship, const std::vector<CelestialBody*>& bodies)
    {
        // Find nearest body
        Vec3 shipPos = ship.getPhysicsBody().position;
        float nearestDist = 999999.0f;
        std::string nearestName = "Unknown";
        float nearestRadius = 0.0f;

        for (auto* body : bodies)
        {
            Vec3 diff = body->getPhysicsBody().position - shipPos;
            float dist = diff.length();
            if (dist < nearestDist)
            {
                nearestDist = dist;
                nearestName = body->getName();
                nearestRadius = body->getRadius();
            }
        }

        float altitude = nearestDist - nearestRadius;

        // Background
        drawList->AddRectFilled(ImVec2(pos.x - 10, pos.y - 10),
                               ImVec2(pos.x + 200, pos.y + 60),
                               IM_COL32(0, 0, 0, 180));

        // Label
        drawList->AddText(ImVec2(pos.x, pos.y), colorNormal, "ALTITUDE");

        // Altitude value with color coding
        char altText[64];
        snprintf(altText, sizeof(altText), "%.1f units", altitude);

        ImU32 altColor;
        if (altitude < 5.0f)
            altColor = colorDanger;
        else if (altitude < 15.0f)
            altColor = colorRetrograde;  // Orange warning
        else
            altColor = colorPrograde;  // Green safe

        drawList->AddText(ImVec2(pos.x, pos.y + 20), altColor, altText);

        // Nearest body label
        char bodyText[64];
        snprintf(bodyText, sizeof(bodyText), "Near: %s", nearestName.c_str());
        drawList->AddText(ImVec2(pos.x, pos.y + 40), IM_COL32(150, 150, 150, 255), bodyText);
    }

    // G-Force indicator (top-right corner)
    void renderGForceIndicator(ImDrawList* drawList, ImVec2 pos, const Spaceship& ship)
    {
        // This would need access to telemetry data
        // For now, show placeholder

        // Background
        drawList->AddRectFilled(ImVec2(pos.x - 150, pos.y - 10),
                               ImVec2(pos.x + 10, pos.y + 60),
                               IM_COL32(0, 0, 0, 180));

        // Label
        drawList->AddText(ImVec2(pos.x - 140, pos.y), colorNormal, "G-FORCE");

        // Placeholder value
        drawList->AddText(ImVec2(pos.x - 140, pos.y + 20), colorNormal, "1.2 G");
        drawList->AddText(ImVec2(pos.x - 140, pos.y + 40), IM_COL32(150, 150, 150, 255), "NOMINAL");
    }

    // Toggle functions
    void toggleTargetingReticle() { showTargetingReticle = !showTargetingReticle; }
    void toggleSpeedVector() { showSpeedVector = !showSpeedVector; }
    void toggleDistanceRuler() { showDistanceRuler = !showDistanceRuler; }
    void toggleNavCompass() { showNavCompass = !showNavCompass; }
    void toggleProgradeRetrograde() { showProgradeRetrograde = !showProgradeRetrograde; }
    void toggleOrbitIndicators() { showOrbitIndicators = !showOrbitIndicators; }

    // Setters
    void setShowAll(bool show)
    {
        showTargetingReticle = show;
        showSpeedVector = show;
        showDistanceRuler = show;
        showNavCompass = show;
        showProgradeRetrograde = show;
        showOrbitIndicators = show;
    }
};
