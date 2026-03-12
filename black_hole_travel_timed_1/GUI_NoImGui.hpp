#pragma once

// TEMPORARY VERSION - Works without ImGui installed
// After installing ImGui, replace this with the full GUI.hpp version

#include "Vec3.hpp"
#include "Spaceship.hpp"
#include "Camera.hpp"
#include "PostProcessing.hpp"
#include <SFML/Window.hpp>
#include <string>
#include <vector>

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

// Stub GUI class that does nothing but maintains state
// This allows the code to compile without ImGui
class GUI
{
private:
    GUIState state;

public:
    GUI(sf::Window& window)
    {
        // No-op constructor
    }

    void processEvent(const sf::Event& event)
    {
        // No-op - no GUI to process
    }

    void update(sf::Window& window)
    {
        // No-op - no GUI to update
    }

    void renderControlPanel()
    {
        // No-op - no GUI to render
    }

    void renderShipTelemetry(const Spaceship& ship, const std::vector<CelestialBody*>& bodies)
    {
        // No-op - no GUI to render
    }

    void renderVisualSettings(PostProcessing* postProcessing)
    {
        // No-op - no GUI to render
    }

    void renderOrbitalData(const Spaceship& ship)
    {
        // No-op - no GUI to render
    }

    void renderPerformanceStats(float fps, float frameTime)
    {
        // No-op - no GUI to render
    }

    void render(sf::Window& window)
    {
        // No-op - no GUI to render
    }

    void shutdown()
    {
        // No-op - no cleanup needed
    }

    // Getters for state
    GUIState& getState() { return state; }
    const GUIState& getState() const { return state; }
};
