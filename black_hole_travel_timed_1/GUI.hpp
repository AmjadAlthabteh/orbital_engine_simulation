#pragma once

// Forward declarations to avoid including imgui headers here
// Users will need to download ImGui from: https://github.com/ocornut/imgui
// Files needed: imgui.h, imgui.cpp, imgui_draw.cpp, imgui_tables.cpp, imgui_widgets.cpp
// Backends: imgui_impl_opengl3.h/cpp, imgui_impl_sfml.h/cpp

#include "Vec3.hpp"
#include "Spaceship.hpp"
#include "Camera.hpp"
#include "PostProcessing.hpp"
#include <SFML/Window.hpp>
#include <string>
#include <vector>

// Include ImGui headers
#include <imgui.h>
#include <imgui-SFML.h>

class CelestialBody;

struct GUIState
{
    // Panels
    bool showControlPanel = true;
    bool showShipTelemetry = true;
    bool showVisualSettings = true;
    bool showOrbitalData = false;
    bool showPerformanceStats = false;

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

    // Ship controls
    float thrustPower = 50.0f;
    float rotationSpeed = 60.0f;

    // Target selection
    int selectedTarget = -1;
    std::vector<std::string> targetNames;

    // Performance stats
    float fps = 0.0f;
    float frameTime = 0.0f;
};

class GUI
{
private:
    GUIState state;
    sf::Clock deltaClock;

public:
    GUI(sf::Window& window)
    {
        ImGui::SFML::Init(window);

        // Set up ImGui style
        setupStyle();
    }

    void setupStyle()
    {
        ImGuiStyle& style = ImGui::GetStyle();

        // Dark space theme
        ImVec4* colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.12f, 0.94f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.08f, 0.08f, 0.15f, 0.00f);
        colors[ImGuiCol_Border] = ImVec4(0.20f, 0.25f, 0.50f, 0.50f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.10f, 0.20f, 0.54f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.25f, 0.50f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.25f, 0.30f, 0.60f, 0.67f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.09f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.15f, 0.30f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.40f, 0.70f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.40f, 0.70f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.50f, 0.80f, 1.00f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.50f, 0.40f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.30f, 0.40f, 0.70f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.25f, 0.35f, 0.65f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.50f, 0.55f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.40f, 0.70f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.35f, 0.65f, 1.00f);

        style.WindowRounding = 5.0f;
        style.FrameRounding = 3.0f;
        style.GrabRounding = 3.0f;
    }

    void processEvent(const sf::Event& event)
    {
        ImGui::SFML::ProcessEvent(event);
    }

    void update(sf::Window& window)
    {
        ImGui::SFML::Update(window, deltaClock.restart());
    }

    void renderControlPanel()
    {
        if (!state.showControlPanel) return;

        ImGui::Begin("Control Panel", &state.showControlPanel);

        ImGui::Text("Black Hole Travel Simulator");
        ImGui::Separator();

        // Time controls
        if (ImGui::CollapsingHeader("Time Controls", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::Button(state.isPaused ? "Resume" : "Pause"))
                state.isPaused = !state.isPaused;

            ImGui::SliderFloat("Time Scale", &state.timeScale, 0.1f, 10.0f, "%.1fx");

            if (ImGui::Button("Reset Time Scale"))
                state.timeScale = 1.0f;
        }

        // Display toggles
        if (ImGui::CollapsingHeader("Display Options"))
        {
            ImGui::Checkbox("Show Velocity Vectors", &state.showVectors);
            ImGui::Checkbox("Show Trajectory", &state.showTrajectory);
            ImGui::Checkbox("Show Coordinate Grid", &state.showGrid);
            ImGui::Checkbox("Show Lagrange Points", &state.showLagrangePoints);
        }

        // Panel toggles
        if (ImGui::CollapsingHeader("Panels"))
        {
            ImGui::Checkbox("Ship Telemetry", &state.showShipTelemetry);
            ImGui::Checkbox("Visual Settings", &state.showVisualSettings);
            ImGui::Checkbox("Orbital Data", &state.showOrbitalData);
            ImGui::Checkbox("Performance Stats", &state.showPerformanceStats);
        }

        ImGui::End();
    }

    void renderShipTelemetry(const Spaceship& ship, const std::vector<CelestialBody*>& bodies)
    {
        if (!state.showShipTelemetry) return;

        ImGui::Begin("Ship Telemetry", &state.showShipTelemetry);

        Vec3 pos = ship.getPhysicsBody().position;
        Vec3 vel = ship.getPhysicsBody().velocity;
        float speed = ship.getSpeed();

        ImGui::Text("Position: (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
        ImGui::Text("Velocity: (%.2f, %.2f, %.2f)", vel.x, vel.y, vel.z);
        ImGui::Text("Speed: %.2f m/s", speed);

        ImGui::Separator();

        // Find nearest body
        float nearestDist = 999999.0f;
        std::string nearestName = "Unknown";
        for (auto* body : bodies)
        {
            Vec3 diff = body->getPhysicsBody().position - pos;
            float dist = diff.length();
            if (dist < nearestDist)
            {
                nearestDist = dist;
                nearestName = body->getName();
            }
        }

        ImGui::Text("Nearest: %s", nearestName.c_str());
        ImGui::Text("Distance: %.1f units", nearestDist);

        // Warning for low altitude
        if (nearestDist < 10.0f)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
            ImGui::Text("WARNING: LOW ALTITUDE!");
            ImGui::PopStyleColor();
        }

        ImGui::Separator();

        // Thrust controls
        ImGui::Text("Thrust Controls:");
        ImGui::SliderFloat("Thrust Power", &state.thrustPower, 10.0f, 200.0f, "%.0f");
        ImGui::SliderFloat("Rotation Speed", &state.rotationSpeed, 10.0f, 120.0f, "%.0f deg/s");

        ImGui::End();
    }

    void renderVisualSettings(PostProcessing* postProcessing)
    {
        if (!state.showVisualSettings) return;

        ImGui::Begin("Visual Settings", &state.showVisualSettings);

        ImGui::Text("Post-Processing Effects");
        ImGui::Separator();

        ImGui::Checkbox("Enable Bloom", &state.bloomEnabled);

        if (state.bloomEnabled && postProcessing)
        {
            if (ImGui::SliderFloat("Bloom Threshold", &state.bloomThreshold, 0.5f, 2.0f))
                postProcessing->setBloomThreshold(state.bloomThreshold);

            if (ImGui::SliderFloat("Bloom Strength", &state.bloomStrength, 0.0f, 0.2f))
                postProcessing->setBloomStrength(state.bloomStrength);
        }

        ImGui::Separator();
        ImGui::Text("HDR Tonemapping");

        if (postProcessing)
        {
            if (ImGui::SliderFloat("Exposure", &state.exposure, 0.1f, 3.0f))
                postProcessing->setExposure(state.exposure);
        }

        if (ImGui::Button("Reset to Defaults"))
        {
            state.bloomThreshold = 1.0f;
            state.bloomStrength = 0.04f;
            state.exposure = 1.0f;
            if (postProcessing)
            {
                postProcessing->setBloomThreshold(state.bloomThreshold);
                postProcessing->setBloomStrength(state.bloomStrength);
                postProcessing->setExposure(state.exposure);
            }
        }

        ImGui::End();
    }

    void renderOrbitalData(const Spaceship& ship)
    {
        if (!state.showOrbitalData) return;

        ImGui::Begin("Orbital Data", &state.showOrbitalData);

        ImGui::Text("Orbital Mechanics");
        ImGui::Separator();

        // This would show calculated orbital parameters
        ImGui::Text("Orbit Type: Elliptical");
        ImGui::Text("Apoapsis: N/A");
        ImGui::Text("Periapsis: N/A");
        ImGui::Text("Eccentricity: N/A");

        ImGui::Separator();

        if (ImGui::Button("Calculate Hohmann Transfer"))
        {
            // Trigger transfer calculation
        }

        ImGui::End();
    }

    void renderPerformanceStats(float fps, float frameTime)
    {
        if (!state.showPerformanceStats) return;

        state.fps = fps;
        state.frameTime = frameTime;

        ImGui::Begin("Performance", &state.showPerformanceStats);

        ImGui::Text("FPS: %.1f", state.fps);
        ImGui::Text("Frame Time: %.2f ms", state.frameTime * 1000.0f);

        ImGui::Separator();

        ImGui::Text("Graphics:");
        ImGui::Text("  Bloom: %s", state.bloomEnabled ? "ON" : "OFF");

        ImGui::End();
    }

    void render(sf::Window& window)
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
};
