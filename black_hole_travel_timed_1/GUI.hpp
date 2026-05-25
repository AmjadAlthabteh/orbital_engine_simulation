#pragma once

// IMPROVED Professional ImGui GUI System for Black Hole Travel Simulator
// Features: Unified panels, better organization, keybind overlay, target locking

#include "Vec3.hpp"
#include "Spaceship.hpp"
#include "Camera.hpp"
#include "PostProcessing.hpp"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

// Include ImGui headers
#include <imgui.h>
#include <imgui-SFML.h>

class CelestialBody;

// Simple tutorial objective for GUI (separate from full MissionSystem)
struct TutorialObjective
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
    Vec3 color;  // For highlighting
};

struct GUIState
{
    // Main panels
    bool showLeftPanel = true;      // Ship Status + Controls
    bool showRightPanel = true;     // Navigation + Radar
    bool showTopHUD = true;         // Critical info overlay
    bool showKeybindOverlay = false; // Press H to toggle
    bool showMissions = true;       // Mission objectives
    bool showSettings = false;      // Visual/Graphics settings

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
    bool lockTarget = false;

    // Camera settings
    int cameraMode = 0; // 0=Free, 1=Follow, 2=Chase
    float cameraDistance = 20.0f;
    float cameraHeight = 10.0f;

    // Performance stats
    float fps = 0.0f;
    float frameTime = 0.0f;

    // Tutorial objectives data
    std::vector<TutorialObjective> objectives;
    int currentMissionIndex = 0;

    // Radar settings
    float radarZoom = 1.0f;
    bool radarShowLabels = true;

    // Collapsible sections
    bool shipStatusExpanded = true;
    bool shipControlsExpanded = true;
    bool navigationExpanded = true;
    bool radarExpanded = true;
    bool missionsExpanded = true;
};

class GUI
{
private:
    GUIState state;
    sf::Clock deltaClock;

    // Visual history for graphs
    std::vector<float> speedHistory;
    std::vector<float> altitudeHistory;
    std::vector<float> fpsHistory;
    const int historySize = 100;

    // Font sizes
    float fontSizeNormal = 16.0f;
    float fontSizeLarge = 20.0f;
    float fontSizeHuge = 24.0f;

    // Colors for theming (brighter for better visibility)
    ImVec4 colorSuccess = ImVec4(0.3f, 1.0f, 0.4f, 1.0f);
    ImVec4 colorWarning = ImVec4(1.0f, 0.85f, 0.0f, 1.0f);
    ImVec4 colorDanger = ImVec4(1.0f, 0.3f, 0.3f, 1.0f);
    ImVec4 colorInfo = ImVec4(0.4f, 0.8f, 1.0f, 1.0f);
    ImVec4 colorHighlight = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);  // Cyan for targets

    // Cached target info
    std::vector<TargetInfo> cachedTargets;
    float targetUpdateTimer = 0.0f;

    struct Layout
    {
        ImVec2 workPos{0.0f, 0.0f};
        ImVec2 workSize{0.0f, 0.0f};
        float margin = 10.0f;
        float topHudHeight = 90.0f;
        float mainPanelTopOffset = 110.0f;

        ImVec2 topHudPos() const { return ImVec2(workPos.x + margin, workPos.y + margin); }
        ImVec2 topHudSize() const { return ImVec2(std::max(0.0f, workSize.x - margin * 2.0f), topHudHeight); }

        float mainPanelY() const { return workPos.y + mainPanelTopOffset; }
        float mainPanelHeight() const { return std::max(0.0f, workPos.y + workSize.y - margin - mainPanelY()); }
    };

    Layout getLayout() const
    {
        Layout layout;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        if (viewport)
        {
            layout.workPos = viewport->WorkPos;
            layout.workSize = viewport->WorkSize;
            return layout;
        }

        ImGuiIO& io = ImGui::GetIO();
        layout.workSize = io.DisplaySize;
        return layout;
    }

    static float clampf(float value, float low, float high)
    {
        return (value < low) ? low : ((value > high) ? high : value);
    }

public:
    GUI(sf::RenderWindow& window)
    {
        ImGui::SFML::Init(window);
        setupStyle();
        initializeMissions();

        // Initialize history buffers
        speedHistory.resize(historySize, 0.0f);
        altitudeHistory.resize(historySize, 0.0f);
        fpsHistory.resize(historySize, 60.0f);
    }

    void setupStyle()
    {
        ImGuiStyle& style = ImGui::GetStyle();
        ImGuiIO& io = ImGui::GetIO();

        // Modern space theme with cyan/blue accents
        ImVec4* colors = style.Colors;

        // Background colors (darker for space theme)
        colors[ImGuiCol_WindowBg] = ImVec4(0.04f, 0.04f, 0.10f, 0.96f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.06f, 0.06f, 0.12f, 0.92f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.18f, 0.98f);

        // Border and frame colors
        colors[ImGuiCol_Border] = ImVec4(0.25f, 0.50f, 0.80f, 0.65f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.18f, 0.28f, 0.75f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.22f, 0.35f, 0.55f, 0.70f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.45f, 0.70f, 0.85f);

        // Title bar colors
        colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.15f, 0.25f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.30f, 0.48f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.08f, 0.12f, 0.20f, 0.85f);

        // Interactive element colors (brighter for visibility)
        colors[ImGuiCol_CheckMark] = ImVec4(0.50f, 0.90f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.45f, 0.75f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.55f, 0.90f, 1.00f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.22f, 0.40f, 0.65f, 0.70f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.60f, 0.90f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.28f, 0.50f, 0.80f, 1.00f);

        // Header colors (for collapsible sections)
        colors[ImGuiCol_Header] = ImVec4(0.22f, 0.35f, 0.55f, 0.75f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.32f, 0.50f, 0.75f, 0.90f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.28f, 0.45f, 0.70f, 1.00f);

        // Tab colors
        colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.30f, 0.48f, 0.85f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.35f, 0.55f, 0.80f, 0.95f);
        colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.45f, 0.70f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.12f, 0.20f, 0.32f, 0.85f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.22f, 0.35f, 0.55f, 1.00f);

        // Separator colors
        colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.50f, 0.80f, 0.60f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.35f, 0.65f, 1.00f, 0.80f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.45f, 0.75f, 1.00f, 1.00f);

        // Text colors (brighter for readability)
        colors[ImGuiCol_Text] = ImVec4(0.98f, 0.98f, 1.00f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.55f, 0.55f, 0.60f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.35f, 0.60f, 0.90f, 0.60f);

        // Scrollbar colors
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.06f, 0.12f, 0.60f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.28f, 0.45f, 0.70f, 0.85f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.38f, 0.60f, 0.90f, 0.95f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.45f, 0.70f, 1.00f, 1.00f);

        // Style settings (larger for better readability)
        style.WindowRounding = 8.0f;
        style.ChildRounding = 6.0f;
        style.FrameRounding = 5.0f;
        style.PopupRounding = 6.0f;
        style.ScrollbarRounding = 8.0f;
        style.GrabRounding = 5.0f;
        style.TabRounding = 5.0f;

        style.WindowBorderSize = 1.5f;
        style.FrameBorderSize = 1.0f;
        style.PopupBorderSize = 1.5f;

        // Larger padding and spacing for better readability
        style.WindowPadding = ImVec2(12, 12);
        style.FramePadding = ImVec2(10, 6);
        style.ItemSpacing = ImVec2(10, 8);
        style.ItemInnerSpacing = ImVec2(8, 6);

        style.ScrollbarSize = 16.0f;
        style.GrabMinSize = 12.0f;

        // Scale everything up slightly for better visibility
        style.ScaleAllSizes(1.1f);
    }

    void initializeMissions()
    {
        state.objectives.clear();

        // Tutorial missions
        state.objectives.push_back({"Master ship controls (I/K for thrust, J/L to turn)", false, 0.0f, "Use I to accelerate, K to brake"});
        state.objectives.push_back({"Reach 30 m/s velocity", false, 0.0f, "Build up speed gradually"});
        state.objectives.push_back({"Navigate to Earth (within 25 units)", false, 0.0f, "Use radar to locate Earth"});
        state.objectives.push_back({"Lock onto a target", false, 0.0f, "Select target and click 'Lock Target'"});
        state.objectives.push_back({"Observe a Hohmann transfer orbit", false, 0.0f, "Press H to calculate transfer"});
        state.objectives.push_back({"Approach Black Hole (safely!)", false, 0.0f, "Get close but don't cross event horizon!"});
    }

    void processEvent(sf::RenderWindow& window, const sf::Event& event)
    {
        ImGui::SFML::ProcessEvent(window, event);

        // Toggle keybind overlay with H key
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1)
        {
            state.showKeybindOverlay = !state.showKeybindOverlay;
        }
    }

    void update(sf::RenderWindow& window)
    {
        ImGui::SFML::Update(window, deltaClock.restart());
    }

    // ===== TOP HUD OVERLAY - Critical Information Always Visible =====
    void renderTopHUD(const Spaceship& ship, const std::vector<CelestialBody*>& bodies)
    {
        if (!state.showTopHUD) return;

        const Layout layout = getLayout();

        // Transparent overlay at top of screen - always locked in place
        ImGui::SetNextWindowPos(layout.topHudPos(), ImGuiCond_Always);
        ImGui::SetNextWindowSize(layout.topHudSize(), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.75f);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

        ImGui::Begin("##TopHUD", nullptr, flags);

        Vec3 shipPos = ship.getPhysicsBody().position;
        float speed = ship.getSpeed();

        const float hudSpacing = ImGui::GetStyle().ItemSpacing.x;
        const float hudChildHeight = 70.0f;
        const float hudChildWidth = std::max(0.0f, (layout.topHudSize().x - hudSpacing * 2.0f) / 3.0f);

        // LEFT: Ship Status
        ImGui::BeginChild("HUD_Left", ImVec2(hudChildWidth, hudChildHeight), false);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);  // Larger font
        ImGui::TextColored(colorInfo, "SHIP STATUS");
        ImGui::PopFont();
        ImGui::Separator();

        ImGui::Text("Speed: ");
        ImGui::SameLine();
        if (speed > 80.0f)
            ImGui::TextColored(colorDanger, "%.1f m/s [HIGH]", speed);
        else if (speed > 50.0f)
            ImGui::TextColored(colorWarning, "%.1f m/s", speed);
        else
            ImGui::TextColored(colorSuccess, "%.1f m/s", speed);

        ImGui::Text("Position: (%.0f, %.0f, %.0f)", shipPos.x, shipPos.y, shipPos.z);
        ImGui::EndChild();

        ImGui::SameLine();

        // CENTER: Nearest Object
        ImGui::BeginChild("HUD_Center", ImVec2(hudChildWidth, hudChildHeight), false);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::TextColored(colorInfo, "PROXIMITY");
        ImGui::PopFont();
        ImGui::Separator();

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

        ImGui::Text("Nearest: %s", nearestName.c_str());
        ImGui::Text("Altitude: ");
        ImGui::SameLine();

        if (altitude < 5.0f)
            ImGui::TextColored(colorDanger, "%.1f units [DANGER!]", altitude);
        else if (altitude < 15.0f)
            ImGui::TextColored(colorWarning, "%.1f units [CAUTION]", altitude);
        else
            ImGui::TextColored(colorSuccess, "%.1f units", altitude);

        ImGui::EndChild();

        ImGui::SameLine();

        // RIGHT: Simulation Status
        ImGui::BeginChild("HUD_Right", ImVec2(hudChildWidth, hudChildHeight), false);
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
        ImGui::TextColored(colorInfo, "SIMULATION");
        ImGui::PopFont();
        ImGui::Separator();

        if (state.isPaused)
            ImGui::TextColored(colorWarning, "STATUS: PAUSED");
        else
            ImGui::TextColored(colorSuccess, "STATUS: RUNNING");

        ImGui::Text("Time Scale: %.1fx", state.timeScale);
        ImGui::Text("FPS: %.0f", state.fps);
        ImGui::EndChild();

        ImGui::End();
    }

    // ===== LEFT PANEL - Ship Status + Controls (Unified) =====
    void renderLeftPanel(const Spaceship& ship, const std::vector<CelestialBody*>& bodies, float deltaTime)
    {
        if (!state.showLeftPanel) return;

        const Layout layout = getLayout();

        const float minCenterWidth = 360.0f;
        float panelWidth = clampf(layout.workSize.x * 0.32f, 340.0f, 460.0f);
        if (layout.workSize.x - layout.margin * 2.0f - panelWidth * 2.0f < minCenterWidth)
        {
            panelWidth = std::max(300.0f, (layout.workSize.x - layout.margin * 2.0f - minCenterWidth) * 0.5f);
        }
        const float maxPanelWidth = std::max(240.0f, layout.workSize.x - layout.margin * 2.0f);
        panelWidth = clampf(panelWidth, 240.0f, maxPanelWidth);

        // Lock panel position and size (cannot be moved or resized)
        ImGui::SetNextWindowPos(ImVec2(layout.workPos.x + layout.margin, layout.mainPanelY()), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(panelWidth, layout.mainPanelHeight()), ImGuiCond_Always);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
        ImGui::Begin("Ship Operations", &state.showLeftPanel, flags);

        Vec3 pos = ship.getPhysicsBody().position;
        Vec3 vel = ship.getPhysicsBody().velocity;
        float speed = ship.getSpeed();

        // Update speed history
        speedHistory.erase(speedHistory.begin());
        speedHistory.push_back(speed);

        // ===== SHIP STATUS SECTION =====
        if (ImGui::CollapsingHeader("Ship Status", state.shipStatusExpanded ? ImGuiTreeNodeFlags_DefaultOpen : 0))
        {
            state.shipStatusExpanded = true;

            ImGui::Indent();

            // Speed graph
            ImGui::TextColored(colorInfo, "Velocity: %.2f m/s", speed);
            ImGui::PlotLines("##SpeedGraph", speedHistory.data(), speedHistory.size(),
                           0, nullptr, 0.0f, 120.0f, ImVec2(-1, 80));

            ImGui::Spacing();

            // Position
            ImGui::Text("Position:");
            ImGui::BulletText("X: %.1f   Y: %.1f   Z: %.1f", pos.x, pos.y, pos.z);

            ImGui::Spacing();

            // Velocity vector
            ImGui::Text("Velocity Vector:");
            ImGui::BulletText("X: %.2f   Y: %.2f   Z: %.2f", vel.x, vel.y, vel.z);

            ImGui::Spacing();

            // Landing status
            if (ship.getLandingState() == LandingState::LANDED)
            {
                ImGui::TextColored(colorSuccess, "STATUS: LANDED");
            }
            else if (ship.getLandingState() == LandingState::APPROACHING)
            {
                ImGui::TextColored(colorWarning, "STATUS: APPROACH MODE");
            }
            else
            {
                ImGui::TextColored(colorInfo, "STATUS: FLIGHT MODE");
            }

            ImGui::Unindent();
        }
        else
        {
            state.shipStatusExpanded = false;
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ===== SHIP CONTROLS SECTION =====
        if (ImGui::CollapsingHeader("Ship Controls", state.shipControlsExpanded ? ImGuiTreeNodeFlags_DefaultOpen : 0))
        {
            state.shipControlsExpanded = true;

            ImGui::Indent();

            // Thrust control
            ImGui::TextColored(colorInfo, "Thrust Power:");
            ImGui::SliderFloat("##Thrust", &state.thrustPower, 10.0f, 200.0f, "%.0f");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Adjust engine thrust power");

            ImGui::Spacing();

            // Rotation speed
            ImGui::TextColored(colorInfo, "Rotation Speed:");
            ImGui::SliderFloat("##Rotation", &state.rotationSpeed, 10.0f, 120.0f, "%.0f deg/s");
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Adjust turning speed");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Quick controls
            ImGui::TextColored(colorInfo, "Quick Actions:");

            if (ImGui::Button("Full Stop", ImVec2(-1, 35)))
            {
                // Would trigger velocity cancellation
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("Cancel all velocity (stop moving)");

            if (state.lockTarget && state.selectedTarget >= 0)
            {
                if (ImGui::Button("Autopilot to Target", ImVec2(-1, 35)))
                {
                    state.autopilotEnabled = !state.autopilotEnabled;
                }
                if (state.autopilotEnabled)
                    ImGui::TextColored(colorSuccess, "AUTOPILOT: ACTIVE");
                else
                    ImGui::TextColored(colorInfo, "AUTOPILOT: STANDBY");
            }
            else
            {
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
                ImGui::Button("Autopilot (No Target)", ImVec2(-1, 35));
                ImGui::PopStyleVar();
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip("Lock a target first");
            }

            ImGui::Unindent();
        }
        else
        {
            state.shipControlsExpanded = false;
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ===== TIME CONTROLS =====
        if (ImGui::CollapsingHeader("Time Control", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Indent();

            // Pause/Resume button (large and prominent)
            if (state.isPaused)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.8f, 0.2f, 0.7f));
            else
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.3f, 0.2f, 0.7f));

            if (ImGui::Button(state.isPaused ? "RESUME" : "PAUSE", ImVec2(-1, 40)))
                state.isPaused = !state.isPaused;
            ImGui::PopStyleColor();

            ImGui::Spacing();

            // Time scale slider
            ImGui::TextColored(colorInfo, "Time Warp:");
            ImGui::SliderFloat("##TimeScale", &state.timeScale, 0.1f, 10.0f, "%.1fx", ImGuiSliderFlags_Logarithmic);

            // Preset buttons
            if (ImGui::Button("0.5x", ImVec2(60, 0))) state.timeScale = 0.5f;
            ImGui::SameLine();
            if (ImGui::Button("1.0x", ImVec2(60, 0))) state.timeScale = 1.0f;
            ImGui::SameLine();
            if (ImGui::Button("2.0x", ImVec2(60, 0))) state.timeScale = 2.0f;
            ImGui::SameLine();
            if (ImGui::Button("5.0x", ImVec2(60, 0))) state.timeScale = 5.0f;

            ImGui::Unindent();
        }

        ImGui::End();
    }

    // ===== RIGHT PANEL - Navigation + Radar (Unified) =====
    void renderRightPanel(const std::vector<CelestialBody*>& bodies, const Spaceship& ship, float deltaTime)
    {
        if (!state.showRightPanel) return;

        const Layout layout = getLayout();

        const float minCenterWidth = 360.0f;
        float panelWidth = clampf(layout.workSize.x * 0.32f, 340.0f, 460.0f);
        if (layout.workSize.x - layout.margin * 2.0f - panelWidth * 2.0f < minCenterWidth)
        {
            panelWidth = std::max(300.0f, (layout.workSize.x - layout.margin * 2.0f - minCenterWidth) * 0.5f);
        }

        // Lock panel position and size (cannot be moved or resized)
        ImGui::SetNextWindowPos(
            ImVec2(layout.workPos.x + layout.workSize.x - layout.margin - panelWidth, layout.mainPanelY()),
            ImGuiCond_Always
        );
        ImGui::SetNextWindowSize(ImVec2(panelWidth, layout.mainPanelHeight()), ImGuiCond_Always);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
        ImGui::Begin("Navigation & Radar", &state.showRightPanel, flags);

        Vec3 shipPos = ship.getPhysicsBody().position;

        // Update cached targets periodically
        targetUpdateTimer += deltaTime;
        if (targetUpdateTimer > 0.5f || cachedTargets.empty())
        {
            updateTargetCache(bodies, ship);
            targetUpdateTimer = 0.0f;
        }

        // ===== NAVIGATION SECTION =====
        if (ImGui::CollapsingHeader("Target Selection", state.navigationExpanded ? ImGuiTreeNodeFlags_DefaultOpen : 0))
        {
            state.navigationExpanded = true;

            ImGui::Indent();

            ImGui::TextColored(colorInfo, "Available Targets:");
            ImGui::Separator();

            // Target list with highlighting
            ImGui::BeginChild("TargetList", ImVec2(-1, 180), true);

            if (ImGui::BeginTable("##TargetTable", 3, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("Target", ImGuiTableColumnFlags_WidthStretch, 2.0f);
                ImGui::TableSetupColumn("Dist", ImGuiTableColumnFlags_WidthFixed, 70.0f);
                ImGui::TableSetupColumn("State", ImGuiTableColumnFlags_WidthFixed, 90.0f);

                for (size_t i = 0; i < cachedTargets.size(); ++i)
                {
                    const TargetInfo& target = cachedTargets[i];
                    const bool isSelected = (state.selectedTarget == static_cast<int>(i));

                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    if (ImGui::Selectable(target.name.c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns))
                    {
                        state.selectedTarget = static_cast<int>(i);
                    }

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextColored(colorInfo, "%.0f u", target.distance);

                    ImGui::TableSetColumnIndex(2);
                    if (isSelected && state.lockTarget)
                    {
                        ImGui::TextColored(colorHighlight, "LOCKED");
                    }
                    else if (target.isLandable)
                    {
                        ImGui::TextColored(colorSuccess, "LAND");
                    }
                    else
                    {
                        ImGui::TextDisabled("-");
                    }
                }

                ImGui::EndTable();
            }

            ImGui::EndChild();

            ImGui::Spacing();

            // Target lock button
            if (state.selectedTarget >= 0 && state.selectedTarget < static_cast<int>(cachedTargets.size()))
            {
                const TargetInfo& target = cachedTargets[state.selectedTarget];

                ImGui::Separator();
                ImGui::TextColored(colorInfo, "Selected: %s", target.name.c_str());
                ImGui::Text("Distance: %.1f units", target.distance);
                ImGui::Text("Rel. Velocity: %.2f m/s", target.relativeVelocity.length());

                ImGui::Spacing();

                // Lock/Unlock button
                if (state.lockTarget)
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.3f, 0.2f, 0.7f));
                    if (ImGui::Button("UNLOCK TARGET", ImVec2(-1, 35)))
                        state.lockTarget = false;
                    ImGui::PopStyleColor();
                }
                else
                {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.7f, 0.3f, 0.7f));
                    if (ImGui::Button("LOCK TARGET", ImVec2(-1, 35)))
                        state.lockTarget = true;
                    ImGui::PopStyleColor();
                }

                if (target.isLandable)
                {
                    if (ImGui::Button("Plan Landing", ImVec2(-1, 30)))
                    {
                        // Would trigger landing sequence
                    }
                }
            }

            ImGui::Unindent();
        }
        else
        {
            state.navigationExpanded = false;
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // ===== RADAR SECTION =====
        if (ImGui::CollapsingHeader("Radar Display", state.radarExpanded ? ImGuiTreeNodeFlags_DefaultOpen : 0))
        {
            state.radarExpanded = true;

            ImGui::Indent();

            // Radar zoom control
            ImGui::Text("Zoom:");
            ImGui::SameLine();
            ImGui::SliderFloat("##RadarZoom", &state.radarZoom, 0.2f, 3.0f, "%.1fx");

            ImGui::Checkbox("Show Labels", &state.radarShowLabels);

            ImGui::Spacing();

            // Radar display
            renderRadar(bodies, ship);

            ImGui::Unindent();
        }
        else
        {
            state.radarExpanded = false;
        }

        ImGui::End();
    }

    // ===== RADAR VISUALIZATION =====
    void renderRadar(const std::vector<CelestialBody*>& bodies, const Spaceship& ship)
    {
        const float maxRadarSize = 360.0f;
        const float availableWidth = std::max(0.0f, ImGui::GetContentRegionAvail().x);
        const float radarSize = std::min(availableWidth, maxRadarSize);

        ImVec2 radarPos = ImGui::GetCursorScreenPos();
        ImVec2 radarCenter(radarPos.x + radarSize * 0.5f, radarPos.y + radarSize * 0.5f);
        float radarRadius = radarSize * 0.45f;

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // Draw radar background circles
        drawList->AddCircleFilled(radarCenter, radarRadius, IM_COL32(10, 15, 25, 200));
        drawList->AddCircle(radarCenter, radarRadius, IM_COL32(60, 120, 180, 150), 64, 3.0f);
        drawList->AddCircle(radarCenter, radarRadius * 0.66f, IM_COL32(50, 100, 160, 100), 64, 2.0f);
        drawList->AddCircle(radarCenter, radarRadius * 0.33f, IM_COL32(40, 80, 140, 80), 64, 1.5f);

        // Draw crosshairs
        drawList->AddLine(ImVec2(radarCenter.x - radarRadius, radarCenter.y),
                         ImVec2(radarCenter.x + radarRadius, radarCenter.y),
                         IM_COL32(60, 120, 180, 120), 2.0f);
        drawList->AddLine(ImVec2(radarCenter.x, radarCenter.y - radarRadius),
                         ImVec2(radarCenter.x, radarCenter.y + radarRadius),
                         IM_COL32(60, 120, 180, 120), 2.0f);

        // Ship at center (larger)
        drawList->AddCircleFilled(radarCenter, 6.0f, IM_COL32(100, 220, 255, 255));
        drawList->AddCircle(radarCenter, 6.0f, IM_COL32(255, 255, 255, 200), 12, 2.0f);

        Vec3 shipPos = ship.getPhysicsBody().position;
        float radarScale = state.radarZoom * 0.6f;

        // Plot celestial bodies
        for (size_t i = 0; i < bodies.size(); ++i)
        {
            auto* body = bodies[i];
            Vec3 relPos = body->getPhysicsBody().position - shipPos;
            float dist = relPos.length();

            // Only show on radar if within range
            if (dist < radarRadius / radarScale)
            {
                float x = radarCenter.x + relPos.x * radarScale;
                float y = radarCenter.y - relPos.z * radarScale;

                // Choose color and size based on object type
                ImU32 color;
                float size;
                std::string label;

                if (body->getName() == "Sun")
                {
                    color = IM_COL32(255, 220, 80, 255);
                    size = 8.0f;
                    label = "SUN";
                }
                else if (body->getName() == "Black Hole")
                {
                    color = IM_COL32(180, 70, 220, 255);
                    size = 9.0f;
                    label = "BH";
                }
                else if (body->getName() == "Earth")
                {
                    color = IM_COL32(80, 180, 255, 255);
                    size = 5.0f;
                    label = "EAR";
                }
                else if (body->getName() == "Mars")
                {
                    color = IM_COL32(255, 120, 80, 255);
                    size = 4.5f;
                    label = "MAR";
                }
                else
                {
                    color = IM_COL32(180, 180, 180, 220);
                    size = 4.0f;
                    label = body->getName().substr(0, 3);
                }

                // Highlight locked target
                bool isLocked = (state.lockTarget && state.selectedTarget >= 0 &&
                               static_cast<size_t>(state.selectedTarget) < cachedTargets.size() &&
                               cachedTargets[state.selectedTarget].name == body->getName());

                if (isLocked)
                {
                    // Pulsing ring around locked target
                    float pulseSize = 12.0f + 3.0f * std::sin(ImGui::GetTime() * 4.0f);
                    drawList->AddCircle(ImVec2(x, y), pulseSize, IM_COL32(0, 255, 255, 200), 16, 2.5f);
                }

                // Draw object
                drawList->AddCircleFilled(ImVec2(x, y), size, color);
                drawList->AddCircle(ImVec2(x, y), size, IM_COL32(255, 255, 255, 150), 12, 1.5f);

                // Draw label if enabled
                if (state.radarShowLabels && size > 4.0f)
                {
                    drawList->AddText(ImVec2(x + size + 3, y - 7), IM_COL32(200, 200, 220, 255), label.c_str());
                }
            }
        }

        ImGui::Dummy(ImVec2(radarSize, radarSize)); // Reserve space
    }

    // ===== MISSION OBJECTIVES PANEL =====
    void renderMissions()
    {
        if (!state.showMissions) return;

        // Lock panel position and size (cannot be moved or resized)
        const Layout layout = getLayout();
        const float desiredWidth = 420.0f;
        const float desiredHeight = 300.0f;
        const float maxWidth = std::max(0.0f, layout.workSize.x - layout.margin * 2.0f);
        const float clampedWidth = std::min(desiredWidth, maxWidth);
        const float x = layout.workPos.x + (layout.workSize.x - clampedWidth) * 0.5f;

        ImGui::SetNextWindowPos(ImVec2(x, layout.mainPanelY()), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(clampedWidth, desiredHeight), ImGuiCond_Always);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
        ImGui::Begin("Mission Objectives", &state.showMissions, flags);

        ImGui::TextColored(colorInfo, "Current Objectives:");
        ImGui::Separator();
        ImGui::Spacing();

        for (size_t i = 0; i < state.objectives.size(); ++i)
        {
            const TutorialObjective& obj = state.objectives[i];

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
                ImGui::Indent(30.0f);
                ImGui::ProgressBar(obj.progress, ImVec2(-1, 0));
                ImGui::Unindent(30.0f);
            }

            ImGui::Spacing();
        }

        ImGui::Separator();

        // Count completed objectives
        int completed = 0;
        for (const TutorialObjective& obj : state.objectives)
            if (obj.completed) completed++;

        ImGui::Text("Overall Progress:");
        ImGui::ProgressBar((float)completed / state.objectives.size(), ImVec2(-1, 0));
        ImGui::Text("%d / %d Objectives Complete", completed, (int)state.objectives.size());

        ImGui::End();
    }

    // ===== KEYBIND OVERLAY (Press F1 to toggle) =====
    void renderKeybindOverlay()
    {
        if (!state.showKeybindOverlay) return;

        // Lock panel position and size (cannot be moved or resized)
        const Layout layout = getLayout();
        const ImVec2 desiredSize(520.0f, 540.0f);
        const float maxWidth = std::max(0.0f, layout.workSize.x - layout.margin * 2.0f);
        const float maxHeight = std::max(0.0f, layout.workSize.y - layout.margin * 2.0f);
        const ImVec2 clampedSize(
            std::min(desiredSize.x, maxWidth),
            std::min(desiredSize.y, maxHeight)
        );
        const ImVec2 pos(
            layout.workPos.x + (layout.workSize.x - clampedSize.x) * 0.5f,
            layout.workPos.y + (layout.workSize.y - clampedSize.y) * 0.5f
        );

        ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(clampedSize, ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.95f);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
        ImGui::Begin("Controls & Keybinds", &state.showKeybindOverlay, flags);

        ImGui::TextColored(colorInfo, "Press F1 to toggle this overlay");
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::BeginTabBar("ControlTabs"))
        {
            if (ImGui::BeginTabItem("Ship Controls"))
            {
                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "THRUST:");
                ImGui::BulletText("I or Up Arrow    - Forward thrust");
                ImGui::BulletText("K or Down Arrow  - Reverse thrust");

                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "ROTATION:");
                ImGui::BulletText("J or Left Arrow  - Rotate left (yaw)");
                ImGui::BulletText("L or Right Arrow - Rotate right (yaw)");
                ImGui::BulletText("U                - Pitch up");
                ImGui::BulletText("O                - Pitch down");

                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "LANDING:");
                ImGui::BulletText("B                - Attempt landing");
                ImGui::BulletText("N                - Take off");

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Camera"))
            {
                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "CAMERA MOVEMENT (Free Mode):");
                ImGui::BulletText("W A S D          - Move camera");
                ImGui::BulletText("Mouse            - Look around");
                ImGui::BulletText("Mouse Wheel      - Zoom (adjust FOV)");

                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "CAMERA MODES:");
                ImGui::BulletText("C                - Cycle camera mode");
                ImGui::BulletText("  • Free Flight");
                ImGui::BulletText("  • Follow Ship");
                ImGui::BulletText("  • Chase Ship");

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Display"))
            {
                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "VISUAL OVERLAYS:");
                ImGui::BulletText("Space            - Toggle velocity vectors");
                ImGui::BulletText("T                - Toggle trajectory markers");
                ImGui::BulletText("R                - Toggle ship trajectory");
                ImGui::BulletText("G                - Toggle coordinate grid");
                ImGui::BulletText("L                - Toggle Lagrange points");

                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "ANALYSIS TOOLS:");
                ImGui::BulletText("H                - Calculate Hohmann transfer");
                ImGui::BulletText("M                - Show orbital mechanics");

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Simulation"))
            {
                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "TIME CONTROLS:");
                ImGui::BulletText("P                - Pause/Resume simulation");
                ImGui::BulletText("+ or =           - Speed up time (up to 10x)");
                ImGui::BulletText("- or _           - Slow down time (down to 0.1x)");

                ImGui::Spacing();
                ImGui::TextColored(colorInfo, "INTERFACE:");
                ImGui::BulletText("F1               - Toggle this help overlay");
                ImGui::BulletText("ESC              - Exit simulation");

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    // ===== VISUAL SETTINGS PANEL =====
    void renderVisualSettings(PostProcessing* postProcessing)
    {
        if (!state.showSettings) return;

        // Lock panel position and size (cannot be moved or resized)
        const Layout layout = getLayout();
        const float desiredWidth = 420.0f;
        const float desiredHeight = 320.0f;
        const float maxWidth = std::max(0.0f, layout.workSize.x - layout.margin * 2.0f);
        const float clampedWidth = std::min(desiredWidth, maxWidth);
        const float x = layout.workPos.x + (layout.workSize.x - clampedWidth) * 0.5f;
        const float y = layout.workPos.y + (layout.workSize.y - desiredHeight) * 0.5f;

        ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(clampedWidth, desiredHeight), ImGuiCond_Always);

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
        ImGui::Begin("Visual Settings", &state.showSettings, flags);

        ImGui::TextColored(colorInfo, "Post-Processing Effects");
        ImGui::Separator();

        // Bloom settings
        if (ImGui::Checkbox("Enable Bloom", &state.bloomEnabled))
        {
            // Bloom toggle
        }

        if (state.bloomEnabled && postProcessing)
        {
            if (ImGui::SliderFloat("Threshold", &state.bloomThreshold, 0.5f, 2.0f, "%.2f"))
                postProcessing->setBloomThreshold(state.bloomThreshold);

            if (ImGui::SliderFloat("Strength", &state.bloomStrength, 0.0f, 0.2f, "%.3f"))
                postProcessing->setBloomStrength(state.bloomStrength);
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
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Display toggles
        ImGui::TextColored(colorInfo, "Visual Overlays");
        ImGui::Checkbox("Velocity Vectors", &state.showVectors);
        ImGui::Checkbox("Trajectory Prediction", &state.showTrajectory);
        ImGui::Checkbox("Orbital Paths", &state.showOrbitalPaths);
        ImGui::Checkbox("Coordinate Grid", &state.showGrid);
        ImGui::Checkbox("Lagrange Points", &state.showLagrangePoints);

        ImGui::End();
    }

    // ===== MAIN RENDER FUNCTION =====
    void renderAllPanels(const Spaceship& ship, const std::vector<CelestialBody*>& bodies,
                         PostProcessing* postProcessing, float fps, float frameTime)
    {
        state.fps = fps;
        state.frameTime = frameTime;

        // Update FPS history
        fpsHistory.erase(fpsHistory.begin());
        fpsHistory.push_back(fps);

        // Render all UI elements
        renderTopHUD(ship, bodies);
        renderLeftPanel(ship, bodies, frameTime);
        renderRightPanel(bodies, ship, frameTime);
        renderMissions();
        renderKeybindOverlay();
        renderVisualSettings(postProcessing);
    }

    void render(sf::RenderWindow& window)
    {
        ImGui::SFML::Render(window);
    }

    void shutdown()
    {
        ImGui::SFML::Shutdown();
    }

    // Getters
    GUIState& getState() { return state; }
    const GUIState& getState() const { return state; }

    // Get locked target information
    bool isTargetLocked() const { return state.lockTarget && state.selectedTarget >= 0; }

    CelestialBody* getLockedTarget(const std::vector<CelestialBody*>& bodies) const
    {
        if (!state.lockTarget || state.selectedTarget < 0 || state.selectedTarget >= static_cast<int>(cachedTargets.size()))
            return nullptr;

        // Find the body in the bodies list matching the cached target name
        std::string targetName = cachedTargets[state.selectedTarget].name;
        for (auto* body : bodies)
        {
            if (body->getName() == targetName)
                return body;
        }
        return nullptr;
    }

    // Update mission progress
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

private:
    // Update cached target information
    void updateTargetCache(const std::vector<CelestialBody*>& bodies, const Spaceship& ship)
    {
        cachedTargets.clear();
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
            info.color = body->getColor();
            cachedTargets.push_back(info);
        }

        // Sort by distance
        std::sort(cachedTargets.begin(), cachedTargets.end(),
                  [](const TargetInfo& a, const TargetInfo& b) { return a.distance < b.distance; });
    }
};
