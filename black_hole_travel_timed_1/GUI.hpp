#pragma once

// Professional ImGui GUI System for Black Hole Travel Simulator
// Features: Multi-panel interface, mission system, radar, advanced controls

#include "Vec3.hpp"
#include "Spaceship.hpp"
#include "Camera.hpp"
#include "PostProcessing.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <cmath>

// Include ImGui headers
#include <imgui.h>
#include <imgui-SFML.h>

class CelestialBody;

struct MissionObjective
{
    std::string description;
    bool completed;
    float progress;  // 0.0 to 1.0
    std::string hint;
};

struct TargetInfo
{
    std::string name;
    Vec3 position;
    float distance;
    Vec3 relativeVelocity;
    bool isLandable;
};

struct GUIState
{
    // Main window toggles
    bool showMainControl = true;
    bool showShipStatus = true;
    bool showNavigation = true;
    bool showMissions = false;
    bool showVisualSettings = false;
    bool showPerformance = false;
    bool showHelp = false;
    bool showRadar = true;

    // Visual settings
    bool bloomEnabled = true;
    float bloomThreshold = 1.0f;
    float bloomStrength = 0.04f;
    float exposure = 1.0f;

    // Physics settings
    float timeScale = 1.0f;
    bool isPaused = false;

    // Display toggles
    bool showVectors = true;
    bool showTrajectory = true;
    bool showGrid = false;
    bool showLagrangePoints = false;
    bool showOrbitalPaths = true;

    // Ship controls
    float thrustPower = 50.0f;
    float rotationSpeed = 60.0f;
    bool autopilotEnabled = false;

    // Target selection
    int selectedTarget = -1;
    std::vector<std::string> targetNames;
    bool lockTarget = false;

    // Camera settings
    int cameraMode = 0; // 0=Free, 1=Follow, 2=Chase
    float cameraDistance = 20.0f;
    float cameraHeight = 10.0f;

    // Performance stats
    float fps = 0.0f;
    float frameTime = 0.0f;

    // Mission data
    std::vector<MissionObjective> objectives;
    int currentMissionIndex = 0;

    // Tutorial state
    bool showTutorial = true;
    int tutorialStep = 0;
};

class GUI
{
private:
    GUIState state;
    sf::Clock deltaClock;

    // Visual history for graphs
    std::vector<float> speedHistory;
    std::vector<float> altitudeHistory;
    const int historySize = 100;

    // Colors for theming
    ImVec4 colorSuccess = ImVec4(0.2f, 1.0f, 0.3f, 1.0f);
    ImVec4 colorWarning = ImVec4(1.0f, 0.8f, 0.0f, 1.0f);
    ImVec4 colorDanger = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
    ImVec4 colorInfo = ImVec4(0.3f, 0.7f, 1.0f, 1.0f);

public:
    GUI(sf::RenderWindow& window)
    {
        ImGui::SFML::Init(window);
        setupStyle();
        initializeMissions();

        // Initialize history buffers
        speedHistory.resize(historySize, 0.0f);
        altitudeHistory.resize(historySize, 0.0f);
    }

    void setupStyle()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGuiIO& io = ImGui::GetIO();

        // Modern space theme with cyan/blue accents
        ImVec4* colors = style.Colors;

        // Background colors
        colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.12f, 0.95f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.08f, 0.15f, 0.90f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.18f, 0.94f);

        // Border and frame colors
        colors[ImGuiCol_Border] = ImVec4(0.20f, 0.40f, 0.70f, 0.50f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.15f, 0.25f, 0.70f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.30f, 0.50f, 0.60f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.40f, 0.65f, 0.80f);

        // Title bar colors
        colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.12f, 0.20f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.25f, 0.40f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.05f, 0.08f, 0.15f, 0.75f);

        // Interactive element colors
        colors[ImGuiCol_CheckMark] = ImVec4(0.40f, 0.80f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.40f, 0.70f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.50f, 0.85f, 1.00f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.35f, 0.60f, 0.60f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.50f, 0.80f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.45f, 0.75f, 1.00f);

        // Header colors
        colors[ImGuiCol_Header] = ImVec4(0.20f, 0.30f, 0.50f, 0.70f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.45f, 0.70f, 0.85f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.40f, 0.65f, 1.00f);

        // Tab colors
        colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.25f, 0.40f, 0.80f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.30f, 0.50f, 0.75f, 0.95f);
        colors[ImGuiCol_TabActive] = ImVec4(0.25f, 0.40f, 0.65f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.15f, 0.25f, 0.80f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.30f, 0.50f, 1.00f);

        // Separator colors
        colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.40f, 0.70f, 0.50f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.30f, 0.60f, 1.00f, 0.75f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.70f, 1.00f, 1.00f);

        // Text colors
        colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.30f, 0.50f, 0.80f, 0.50f);

        // Scrollbar colors
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.50f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.40f, 0.65f, 0.80f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.55f, 0.85f, 0.90f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.40f, 0.65f, 1.00f, 1.00f);

        // Style settings
        style.WindowRounding = 6.0f;
        style.ChildRounding = 4.0f;
        style.FrameRounding = 4.0f;
        style.PopupRounding = 4.0f;
        style.ScrollbarRounding = 6.0f;
        style.GrabRounding = 4.0f;
        style.TabRounding = 4.0f;

        style.WindowBorderSize = 1.0f;
        style.FrameBorderSize = 1.0f;
        style.PopupBorderSize = 1.0f;

        style.WindowPadding = ImVec2(10, 10);
        style.FramePadding = ImVec2(8, 4);
        style.ItemSpacing = ImVec2(8, 6);
        style.ItemInnerSpacing = ImVec2(6, 4);

        style.ScrollbarSize = 14.0f;
        style.GrabMinSize = 10.0f;
    }

    void initializeMissions()
    {
        state.objectives.clear();

        // Tutorial missions
        state.objectives.push_back({"Learn basic controls (WASD, Mouse)", false, 0.0f, "Move camera around to explore"});
        state.objectives.push_back({"Activate ship thrust (Press I)", false, 0.0f, "Use I key to accelerate forward"});
        state.objectives.push_back({"Rotate ship (J/L keys)", false, 0.0f, "Turn left with J, right with L"});
        state.objectives.push_back({"Reach 50 m/s velocity", false, 0.0f, "Thrust forward until speed reaches 50"});
        state.objectives.push_back({"Get within 20 units of Earth", false, 0.0f, "Navigate toward Earth"});
        state.objectives.push_back({"Observe orbital mechanics", false, 0.0f, "Watch planets orbit the sun"});
        state.objectives.push_back({"Approach Black Hole (carefully!)", false, 0.0f, "Get close but don't get pulled in!"});
    }

    void processEvent(sf::RenderWindow& window, const sf::Event& event)
    {
        ImGui::SFML::ProcessEvent(window, event);
    }

    void update(sf::RenderWindow& window)
    {
        ImGui::SFML::Update(window, deltaClock.restart());
    }

    // Main control panel with tabs
    void renderMainControl()
    {
        if (!state.showMainControl) return;

        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(380, 450), ImGuiCond_FirstUseEver);

        ImGui::Begin("Mission Control", &state.showMainControl, ImGuiWindowFlags_NoCollapse);

        // Title with icon
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Default font but bold effect
        ImGui::TextColored(colorInfo, "BLACK HOLE TRAVEL SIMULATOR");
        ImGui::PopFont();
        ImGui::Separator();

        if (ImGui::BeginTabBar("MainControlTabs"))
        {
            // TIME CONTROL TAB
            if (ImGui::BeginTabItem("Time & Physics"))
            {
                ImGui::Spacing();

                // Pause button (large and prominent)
                if (state.isPaused)
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.2f, 0.6f));
                else
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.3f, 0.2f, 0.6f));

                if (ImGui::Button(state.isPaused ? "RESUME SIMULATION" : "PAUSE SIMULATION", ImVec2(-1, 40)))
                    state.isPaused = !state.isPaused;
                ImGui::PopStyleColor();

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                // Time scale with presets
                ImGui::Text("Time Warp:");
                ImGui::SliderFloat("##TimeScale", &state.timeScale, 0.1f, 10.0f, "%.1fx", ImGuiSliderFlags_Logarithmic);

                if (ImGui::Button("0.1x", ImVec2(60, 0))) state.timeScale = 0.1f;
                ImGui::SameLine();
                if (ImGui::Button("0.5x", ImVec2(60, 0))) state.timeScale = 0.5f;
                ImGui::SameLine();
                if (ImGui::Button("1.0x", ImVec2(60, 0))) state.timeScale = 1.0f;
                ImGui::SameLine();
                if (ImGui::Button("2.0x", ImVec2(60, 0))) state.timeScale = 2.0f;
                ImGui::SameLine();
                if (ImGui::Button("5.0x", ImVec2(60, 0))) state.timeScale = 5.0f;

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                // Display options
                ImGui::TextColored(colorInfo, "Visual Overlays:");
                ImGui::Checkbox("Velocity Vectors", &state.showVectors);
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Show direction and magnitude of velocities");

                ImGui::Checkbox("Trajectory Prediction", &state.showTrajectory);
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Predict future paths of objects");

                ImGui::Checkbox("Orbital Paths", &state.showOrbitalPaths);
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Show calculated orbital ellipses");

                ImGui::Checkbox("Coordinate Grid", &state.showGrid);
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("3D reference grid for navigation");

                ImGui::Checkbox("Lagrange Points", &state.showLagrangePoints);
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Gravitational equilibrium points (L1-L5)");

                ImGui::EndTabItem();
            }

            // CAMERA CONTROL TAB
            if (ImGui::BeginTabItem("Camera"))
            {
                ImGui::Spacing();

                ImGui::TextColored(colorInfo, "Camera Mode:");

                if (ImGui::RadioButton("Free Flight", &state.cameraMode, 0))
                {
                    // Signal camera mode change
                }
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("WASD to move, Mouse to look around");

                if (ImGui::RadioButton("Follow Ship", &state.cameraMode, 1))
                {
                    // Signal camera mode change
                }
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Camera follows ship from behind");

                if (ImGui::RadioButton("Chase Ship", &state.cameraMode, 2))
                {
                    // Signal camera mode change
                }
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Dynamic chase camera with smooth movement");

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                // Camera settings (for follow/chase modes)
                if (state.cameraMode != 0)
                {
                    ImGui::TextColored(colorInfo, "Camera Settings:");
                    ImGui::SliderFloat("Distance", &state.cameraDistance, 5.0f, 100.0f, "%.1f");
                    ImGui::SliderFloat("Height", &state.cameraHeight, -20.0f, 50.0f, "%.1f");
                }

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                ImGui::TextColored(colorInfo, "Quick Actions:");
                if (ImGui::Button("Focus on Sun", ImVec2(-1, 0)))
                {
                    // Would trigger camera focus
                }
                if (ImGui::Button("Focus on Earth", ImVec2(-1, 0)))
                {
                    // Would trigger camera focus
                }
                if (ImGui::Button("Focus on Black Hole", ImVec2(-1, 0)))
                {
                    // Would trigger camera focus
                }

                ImGui::EndTabItem();
            }

            // WINDOWS TAB
            if (ImGui::BeginTabItem("Panels"))
            {
                ImGui::Spacing();

                ImGui::TextColored(colorInfo, "Active Panels:");
                ImGui::Checkbox("Ship Status", &state.showShipStatus);
                ImGui::Checkbox("Navigation & Targets", &state.showNavigation);
                ImGui::Checkbox("Radar Display", &state.showRadar);
                ImGui::Checkbox("Mission Objectives", &state.showMissions);
                ImGui::Checkbox("Visual Settings", &state.showVisualSettings);
                ImGui::Checkbox("Performance Stats", &state.showPerformance);
                ImGui::Checkbox("Help & Controls", &state.showHelp);

                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();

                if (ImGui::Button("Reset Panel Positions", ImVec2(-1, 0)))
                {
                    // Reset window positions to defaults
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    // Ship status panel with detailed telemetry
    void renderShipStatus(const Spaceship& ship, const std::vector<CelestialBody*>& bodies)
    {
        if (!state.showShipStatus) return;

        ImGui::SetNextWindowPos(ImVec2(10, 475), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(380, 350), ImGuiCond_FirstUseEver);

        ImGui::Begin("Ship Status", &state.showShipStatus);

        Vec3 pos = ship.getPhysicsBody().position;
        Vec3 vel = ship.getPhysicsBody().velocity;
        float speed = ship.getSpeed();

        // Update speed history
        speedHistory.erase(speedHistory.begin());
        speedHistory.push_back(speed);

        // Ship state indicator
        ImGui::TextColored(colorSuccess, "SHIP SYSTEMS ONLINE");
        ImGui::Separator();

        // Position and velocity
        ImGui::Text("Position:");
        ImGui::BulletText("X: %.1f", pos.x);
        ImGui::SameLine(150);
        ImGui::BulletText("Y: %.1f", pos.y);
        ImGui::SameLine(280);
        ImGui::BulletText("Z: %.1f", pos.z);

        ImGui::Spacing();
        ImGui::Text("Velocity:");
        ImGui::BulletText("X: %.2f", vel.x);
        ImGui::SameLine(150);
        ImGui::BulletText("Y: %.2f", vel.y);
        ImGui::SameLine(280);
        ImGui::BulletText("Z: %.2f", vel.z);

        ImGui::Spacing();
        ImGui::Separator();

        // Speed with graph
        ImGui::TextColored(colorInfo, "Speed: %.2f m/s", speed);
        ImGui::PlotLines("##SpeedHistory", speedHistory.data(), speedHistory.size(),
                         0, nullptr, 0.0f, 100.0f, ImVec2(0, 60));

        ImGui::Spacing();

        // Find nearest body
        float nearestDist = 999999.0f;
        std::string nearestName = "Unknown";
        float nearestRadius = 0.0f;

        for (auto* body : bodies)
        {
            Vec3 diff = body->getPhysicsBody().position - pos;
            float dist = diff.length();
            if (dist < nearestDist)
            {
                nearestDist = dist;
                nearestName = body->getName();
                nearestRadius = body->getRadius();
            }
        }

        float altitude = nearestDist - nearestRadius;

        // Update altitude history
        altitudeHistory.erase(altitudeHistory.begin());
        altitudeHistory.push_back(altitude);

        ImGui::Separator();
        ImGui::Text("Nearest Object: %s", nearestName.c_str());
        ImGui::Text("Distance: %.1f units", nearestDist);

        // Altitude with color coding
        if (altitude < 5.0f)
            ImGui::TextColored(colorDanger, "Altitude: %.1f units - DANGER!", altitude);
        else if (altitude < 15.0f)
            ImGui::TextColored(colorWarning, "Altitude: %.1f units - CAUTION", altitude);
        else
            ImGui::TextColored(colorSuccess, "Altitude: %.1f units", altitude);

        // Collision warning
        if (altitude < 5.0f && altitude > 0.0f)
        {
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, colorDanger);
            ImGui::TextWrapped("WARNING: COLLISION IMMINENT!");
            float timeToImpact = altitude / (speed + 0.01f);
            ImGui::Text("Time to impact: %.1f seconds", timeToImpact);
            ImGui::PopStyleColor();
        }

        // Black hole warning
        if (nearestName == "Black Hole" && nearestDist < 30.0f)
        {
            ImGui::Spacing();
            ImGui::PushStyleColor(ImGuiCol_Text, colorDanger);
            ImGui::TextWrapped("EXTREME GRAVITATIONAL FORCES DETECTED!");
            ImGui::TextWrapped("Black hole event horizon proximity alert!");
            ImGui::PopStyleColor();
        }

        ImGui::Separator();

        // Ship controls
        ImGui::TextColored(colorInfo, "Ship Controls:");
        ImGui::SliderFloat("Thrust Power", &state.thrustPower, 10.0f, 200.0f, "%.0f");
        ImGui::SliderFloat("Rotation Speed", &state.rotationSpeed, 10.0f, 120.0f, "%.0f deg/s");

        ImGui::Checkbox("Autopilot", &state.autopilotEnabled);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Automatic navigation to selected target");

        ImGui::End();
    }

    // Navigation and target selection
    void renderNavigation(const std::vector<CelestialBody*>& bodies, const Spaceship& ship)
    {
        if (!state.showNavigation) return;

        ImGui::SetNextWindowPos(ImVec2(400, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(350, 400), ImGuiCond_FirstUseEver);

        ImGui::Begin("Navigation", &state.showNavigation);

        ImGui::TextColored(colorInfo, "Target Selection:");
        ImGui::Separator();

        // Build target list
        std::vector<TargetInfo> targets;
        Vec3 shipPos = ship.getPhysicsBody().position;

        for (auto* body : bodies)
        {
            TargetInfo info;
            info.name = body->getName();
            info.position = body->getPhysicsBody().position;
            Vec3 diff = info.position - shipPos;
            info.distance = diff.length();
            info.relativeVelocity = body->getPhysicsBody().velocity - ship.getPhysicsBody().velocity;
            info.isLandable = (info.name == "Earth" || info.name == "Moon" || info.name == "Mars");
            targets.push_back(info);
        }

        // Sort by distance
        std::sort(targets.begin(), targets.end(),
                  [](const TargetInfo& a, const TargetInfo& b) { return a.distance < b.distance; });

        // Target list
        for (size_t i = 0; i < targets.size(); ++i)
        {
            const TargetInfo& target = targets[i];

            bool isSelected = (state.selectedTarget == static_cast<int>(i));

            if (ImGui::Selectable(target.name.c_str(), isSelected))
            {
                state.selectedTarget = i;
            }

            ImGui::SameLine(180);
            ImGui::TextColored(colorInfo, "%.1f units", target.distance);

            if (target.isLandable)
            {
                ImGui::SameLine();
                ImGui::TextColored(colorSuccess, "[LANDABLE]");
            }
        }

        ImGui::Separator();

        // Selected target details
        if (state.selectedTarget >= 0 && state.selectedTarget < static_cast<int>(targets.size()))
        {
            const TargetInfo& target = targets[state.selectedTarget];

            ImGui::TextColored(colorInfo, "Target: %s", target.name.c_str());
            ImGui::Text("Distance: %.1f units", target.distance);
            ImGui::Text("Position: (%.1f, %.1f, %.1f)",
                       target.position.x, target.position.y, target.position.z);
            ImGui::Text("Relative Velocity: %.2f m/s", target.relativeVelocity.length());

            ImGui::Spacing();
            ImGui::Checkbox("Lock Target", &state.lockTarget);

            if (ImGui::Button("Calculate Intercept Course", ImVec2(-1, 0)))
            {
                // Would trigger intercept calculation
            }

            if (target.isLandable)
            {
                if (ImGui::Button("Plan Landing Approach", ImVec2(-1, 0)))
                {
                    // Would trigger landing sequence
                }
            }
        }

        ImGui::End();
    }

    // Radar display (minimap)
    void renderRadar(const std::vector<CelestialBody*>& bodies, const Spaceship& ship)
    {
        if (!state.showRadar) return;

        ImGui::SetNextWindowPos(ImVec2(760, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(250, 250), ImGuiCond_FirstUseEver);

        ImGui::Begin("Radar", &state.showRadar);

        ImVec2 radarCenter = ImGui::GetCursorScreenPos();
        radarCenter.x += 115;
        radarCenter.y += 115;
        float radarRadius = 100.0f;

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Draw radar background circles
        drawList->AddCircle(radarCenter, radarRadius, IM_COL32(50, 100, 150, 100), 64, 2.0f);
        drawList->AddCircle(radarCenter, radarRadius * 0.66f, IM_COL32(40, 80, 120, 80), 64, 1.5f);
        drawList->AddCircle(radarCenter, radarRadius * 0.33f, IM_COL32(30, 60, 100, 60), 64, 1.0f);

        // Draw crosshairs
        drawList->AddLine(ImVec2(radarCenter.x - radarRadius, radarCenter.y),
                         ImVec2(radarCenter.x + radarRadius, radarCenter.y),
                         IM_COL32(50, 100, 150, 80), 1.0f);
        drawList->AddLine(ImVec2(radarCenter.x, radarCenter.y - radarRadius),
                         ImVec2(radarCenter.x, radarCenter.y + radarRadius),
                         IM_COL32(50, 100, 150, 80), 1.0f);

        // Ship is at center
        drawList->AddCircleFilled(radarCenter, 4.0f, IM_COL32(100, 200, 255, 255));

        Vec3 shipPos = ship.getPhysicsBody().position;
        float radarScale = 0.5f; // Adjust for radar zoom

        // Plot celestial bodies
        for (auto* body : bodies)
        {
            Vec3 relPos = body->getPhysicsBody().position - shipPos;
            float dist = relPos.length();

            // Only show on radar if within range
            if (dist < radarRadius / radarScale)
            {
                float x = radarCenter.x + relPos.x * radarScale;
                float y = radarCenter.y - relPos.z * radarScale; // Use Z for 2D projection

                // Choose color based on object type
                ImU32 color;
                float size;

                if (body->getName() == "Sun")
                {
                    color = IM_COL32(255, 200, 50, 255);
                    size = 6.0f;
                }
                else if (body->getName() == "Black Hole")
                {
                    color = IM_COL32(150, 50, 200, 255);
                    size = 7.0f;
                }
                else if (body->getName().find("Earth") != std::string::npos)
                {
                    color = IM_COL32(50, 150, 255, 255);
                    size = 4.0f;
                }
                else
                {
                    color = IM_COL32(150, 150, 150, 200);
                    size = 3.0f;
                }

                drawList->AddCircleFilled(ImVec2(x, y), size, color);
            }
        }

        ImGui::Dummy(ImVec2(230, 230)); // Reserve space

        ImGui::End();
    }

    // Mission objectives panel
    void renderMissions()
    {
        if (!state.showMissions) return;

        ImGui::SetNextWindowPos(ImVec2(400, 420), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(350, 300), ImGuiCond_FirstUseEver);

        ImGui::Begin("Mission Objectives", &state.showMissions);

        ImGui::TextColored(colorInfo, "Active Missions:");
        ImGui::Separator();

        for (size_t i = 0; i < state.objectives.size(); ++i)
        {
            const MissionObjective& obj = state.objectives[i];

            // Checkbox for completion
            bool completed = obj.completed;
            if (ImGui::Checkbox(("##obj" + std::to_string(i)).c_str(), &completed))
            {
                state.objectives[i].completed = completed;
            }

            ImGui::SameLine();

            // Objective text with color
            if (obj.completed)
                ImGui::TextColored(colorSuccess, "%s", obj.description.c_str());
            else
                ImGui::Text("%s", obj.description.c_str());

            // Show hint when hovered
            if (ImGui::IsItemHovered() && !obj.hint.empty())
            {
                ImGui::SetTooltip("%s", obj.hint.c_str());
            }

            // Progress bar if partially complete
            if (obj.progress > 0.0f && !obj.completed)
            {
                ImGui::Indent(20.0f);
                ImGui::ProgressBar(obj.progress, ImVec2(-1, 0));
                ImGui::Unindent(20.0f);
            }

            ImGui::Spacing();
        }

        ImGui::Separator();

        // Count completed objectives
        int completed = 0;
        for (const auto& obj : state.objectives)
            if (obj.completed) completed++;

        ImGui::Text("Progress: %d / %d", completed, (int)state.objectives.size());
        ImGui::ProgressBar((float)completed / state.objectives.size(), ImVec2(-1, 0));

        ImGui::End();
    }

    // Visual settings panel
    void renderVisualSettings(PostProcessing* postProcessing)
    {
        if (!state.showVisualSettings) return;

        ImGui::SetNextWindowPos(ImVec2(760, 270), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(300, 350), ImGuiCond_FirstUseEver);

        ImGui::Begin("Visual Settings", &state.showVisualSettings);

        ImGui::TextColored(colorInfo, "Post-Processing Effects");
        ImGui::Separator();

        // Bloom settings
        if (ImGui::Checkbox("Enable Bloom", &state.bloomEnabled))
        {
            // Bloom toggle
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Creates glow around bright objects");

        if (state.bloomEnabled && postProcessing)
        {
            if (ImGui::SliderFloat("Threshold", &state.bloomThreshold, 0.5f, 2.0f, "%.2f"))
                postProcessing->setBloomThreshold(state.bloomThreshold);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Brightness level that triggers bloom");

            if (ImGui::SliderFloat("Strength", &state.bloomStrength, 0.0f, 0.2f, "%.3f"))
                postProcessing->setBloomStrength(state.bloomStrength);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Intensity of the bloom effect");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // HDR settings
        ImGui::TextColored(colorInfo, "HDR Tone Mapping");

        if (postProcessing)
        {
            if (ImGui::SliderFloat("Exposure", &state.exposure, 0.1f, 3.0f, "%.2f"))
                postProcessing->setExposure(state.exposure);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Overall scene brightness");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Preset buttons
        ImGui::TextColored(colorInfo, "Visual Presets:");

        if (ImGui::Button("Cinematic", ImVec2(-1, 0)))
        {
            state.bloomThreshold = 0.8f;
            state.bloomStrength = 0.08f;
            state.exposure = 1.2f;
            if (postProcessing)
            {
                postProcessing->setBloomThreshold(state.bloomThreshold);
                postProcessing->setBloomStrength(state.bloomStrength);
                postProcessing->setExposure(state.exposure);
            }
        }

        if (ImGui::Button("Realistic", ImVec2(-1, 0)))
        {
            state.bloomThreshold = 1.2f;
            state.bloomStrength = 0.04f;
            state.exposure = 1.0f;
            if (postProcessing)
            {
                postProcessing->setBloomThreshold(state.bloomThreshold);
                postProcessing->setBloomStrength(state.bloomStrength);
                postProcessing->setExposure(state.exposure);
            }
        }

        if (ImGui::Button("High Energy", ImVec2(-1, 0)))
        {
            state.bloomThreshold = 0.5f;
            state.bloomStrength = 0.12f;
            state.exposure = 1.5f;
            if (postProcessing)
            {
                postProcessing->setBloomThreshold(state.bloomThreshold);
                postProcessing->setBloomStrength(state.bloomStrength);
                postProcessing->setExposure(state.exposure);
            }
        }

        if (ImGui::Button("Reset to Defaults", ImVec2(-1, 0)))
        {
            state.bloomThreshold = 1.0f;
            state.bloomStrength = 0.04f;
            state.exposure = 1.0f;
            state.bloomEnabled = true;
            if (postProcessing)
            {
                postProcessing->setBloomThreshold(state.bloomThreshold);
                postProcessing->setBloomStrength(state.bloomStrength);
                postProcessing->setExposure(state.exposure);
            }
        }

        ImGui::End();
    }

    // Performance statistics
    void renderPerformance(float fps, float frameTime)
    {
        if (!state.showPerformance) return;

        state.fps = fps;
        state.frameTime = frameTime;

        ImGui::SetNextWindowPos(ImVec2(1070, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);

        ImGui::Begin("Performance", &state.showPerformance);

        // FPS with color coding
        ImVec4 fpsColor;
        if (fps >= 60.0f)
            fpsColor = colorSuccess;
        else if (fps >= 30.0f)
            fpsColor = colorWarning;
        else
            fpsColor = colorDanger;

        ImGui::TextColored(fpsColor, "FPS: %.1f", state.fps);
        ImGui::Text("Frame: %.2f ms", state.frameTime * 1000.0f);

        ImGui::Separator();

        ImGui::Text("Graphics:");
        ImGui::BulletText("Bloom: %s", state.bloomEnabled ? "ON" : "OFF");
        ImGui::BulletText("Vectors: %s", state.showVectors ? "ON" : "OFF");
        ImGui::BulletText("Grid: %s", state.showGrid ? "ON" : "OFF");

        ImGui::Separator();

        ImGui::Text("Time Scale: %.1fx", state.timeScale);
        ImGui::Text("Status: %s", state.isPaused ? "PAUSED" : "RUNNING");

        ImGui::End();
    }

    // Help and keyboard shortcuts
    void renderHelp()
    {
        if (!state.showHelp) return;

        ImGui::SetNextWindowPos(ImVec2(1070, 220), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(350, 500), ImGuiCond_FirstUseEver);

        ImGui::Begin("Help & Controls", &state.showHelp);

        if (ImGui::BeginTabBar("HelpTabs"))
        {
            if (ImGui::BeginTabItem("Ship Controls"))
            {
                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "Thrust:");
                ImGui::BulletText("I / Up Arrow: Forward thrust");
                ImGui::BulletText("K / Down Arrow: Reverse thrust");

                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "Rotation:");
                ImGui::BulletText("J / Left Arrow: Rotate left (yaw)");
                ImGui::BulletText("L / Right Arrow: Rotate right (yaw)");
                ImGui::BulletText("U: Pitch up");
                ImGui::BulletText("O: Pitch down");

                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "Landing:");
                ImGui::BulletText("B: Attempt landing");
                ImGui::BulletText("N: Take off");

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Camera"))
            {
                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "Movement (Free Mode):");
                ImGui::BulletText("WASD: Move camera");
                ImGui::BulletText("Mouse: Look around");
                ImGui::BulletText("Mouse Wheel: Zoom (FOV)");

                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "Modes:");
                ImGui::BulletText("C: Toggle camera mode");
                ImGui::BulletText("  - Free Flight");
                ImGui::BulletText("  - Follow Ship");
                ImGui::BulletText("  - Chase Ship");

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Display"))
            {
                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "Overlays:");
                ImGui::BulletText("Space: Toggle velocity vectors");
                ImGui::BulletText("T: Toggle trajectory markers");
                ImGui::BulletText("R: Toggle ship trajectory");
                ImGui::BulletText("G: Toggle coordinate grid");
                ImGui::BulletText("L: Toggle Lagrange points");

                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "Analysis:");
                ImGui::BulletText("H: Calculate Hohmann transfer");
                ImGui::BulletText("M: Show orbital mechanics");

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Time"))
            {
                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "Time Controls:");
                ImGui::BulletText("P: Pause/Resume");
                ImGui::BulletText("+/=: Speed up time");
                ImGui::BulletText("-/_: Slow down time");

                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "Other:");
                ImGui::BulletText("ESC: Exit simulation");

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    // Main render function that calls all panels
    void renderAllPanels(const Spaceship& ship, const std::vector<CelestialBody*>& bodies,
                         PostProcessing* postProcessing, float fps, float frameTime)
    {
        renderMainControl();
        renderShipStatus(ship, bodies);
        renderNavigation(bodies, ship);
        renderRadar(bodies, ship);
        renderMissions();
        renderVisualSettings(postProcessing);
        renderPerformance(fps, frameTime);
        renderHelp();
    }

    void render(sf::RenderWindow& window)
    {
        ImGui::SFML::Render(window);
    }

    void shutdown()
    {
        ImGui::SFML::Shutdown();
    }

    // Getters for state
    GUIState& getState() { return state; }
    const GUIState& getState() const { return state; }

    // Update mission progress (called from game logic)
    void updateMissionProgress(int index, float progress)
    {
        if (index >= 0 && index < static_cast<int>(state.objectives.size()))
        {
            state.objectives[index].progress = progress;
            if (progress >= 1.0f)
                state.objectives[index].completed = true;
        }
    }

    void completeMission(int index)
    {
        if (index >= 0 && index < static_cast<int>(state.objectives.size()))
        {
            state.objectives[index].completed = true;
            state.objectives[index].progress = 1.0f;
        }
    }
};
