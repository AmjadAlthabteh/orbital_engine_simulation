#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "Vec3.hpp"

// Science experiment types
enum class ExperimentType
{
    VISUAL_SURVEY,      // Photograph planet
    ATMOSPHERIC_SCAN,   // Scan atmosphere
    GRAVITY_MEASUREMENT,// Measure gravity
    MAGNETIC_FIELD,     // Measure magnetic field
    CLOSE_FLYBY,        // Close approach science
    ORBITAL_STUDY       // Study from orbit
};

struct ScienceData
{
    std::string planetName;
    ExperimentType type;
    float scienceValue;
    std::string description;
    bool collected;
    float distance;  // Distance at which data was collected

    ScienceData(const std::string& name, ExperimentType t, float value, const std::string& desc)
        : planetName(name), type(t), scienceValue(value), description(desc),
          collected(false), distance(0.0f)
    {}
};

class ScienceSystem
{
private:
    std::vector<ScienceData> availableExperiments;
    std::unordered_map<std::string, bool> planetScanned;  // Track which planets have been scanned
    float totalSciencePoints;
    int experimentsCompleted;

    // Scanning state
    bool isScanning;
    float scanProgress;
    std::string currentScanTarget;
    float scanDuration;

public:
    ScienceSystem() : totalSciencePoints(0.0f), experimentsCompleted(0),
                     isScanning(false), scanProgress(0.0f), scanDuration(3.0f)
    {
        initializeExperiments();
    }

    void initializeExperiments()
    {
        // Earth experiments
        availableExperiments.push_back(ScienceData("Earth", ExperimentType::VISUAL_SURVEY,
            15.0f, "High-resolution imaging of Earth's surface and clouds"));
        availableExperiments.push_back(ScienceData("Earth", ExperimentType::ATMOSPHERIC_SCAN,
            20.0f, "Detailed atmospheric composition analysis"));
        availableExperiments.push_back(ScienceData("Earth", ExperimentType::GRAVITY_MEASUREMENT,
            10.0f, "Precise gravitational field mapping"));

        // Moon experiments
        availableExperiments.push_back(ScienceData("Moon", ExperimentType::VISUAL_SURVEY,
            12.0f, "Lunar surface crater mapping"));
        availableExperiments.push_back(ScienceData("Moon", ExperimentType::GRAVITY_MEASUREMENT,
            8.0f, "Moon gravity anomaly detection"));

        // Mars experiments
        availableExperiments.push_back(ScienceData("Mars", ExperimentType::VISUAL_SURVEY,
            18.0f, "Mars surface geology imaging"));
        availableExperiments.push_back(ScienceData("Mars", ExperimentType::ATMOSPHERIC_SCAN,
            25.0f, "Martian atmosphere CO2 analysis"));
        availableExperiments.push_back(ScienceData("Mars", ExperimentType::CLOSE_FLYBY,
            30.0f, "Low-altitude Mars flyby data collection"));

        // Jupiter experiments
        availableExperiments.push_back(ScienceData("Jupiter", ExperimentType::VISUAL_SURVEY,
            22.0f, "Great Red Spot storm analysis"));
        availableExperiments.push_back(ScienceData("Jupiter", ExperimentType::ATMOSPHERIC_SCAN,
            35.0f, "Gas giant atmospheric composition study"));
        availableExperiments.push_back(ScienceData("Jupiter", ExperimentType::MAGNETIC_FIELD,
            40.0f, "Jupiter's powerful magnetic field measurement"));

        // Saturn experiments
        availableExperiments.push_back(ScienceData("Saturn", ExperimentType::VISUAL_SURVEY,
            25.0f, "Saturn ring structure imaging"));
        availableExperiments.push_back(ScienceData("Saturn", ExperimentType::ORBITAL_STUDY,
            45.0f, "Ring particle composition analysis"));

        // Venus experiments
        availableExperiments.push_back(ScienceData("Venus", ExperimentType::ATMOSPHERIC_SCAN,
            30.0f, "Dense Venusian atmosphere study"));
        availableExperiments.push_back(ScienceData("Venus", ExperimentType::VISUAL_SURVEY,
            20.0f, "Volcanic activity detection"));

        // Mercury experiments
        availableExperiments.push_back(ScienceData("Mercury", ExperimentType::VISUAL_SURVEY,
            15.0f, "Heavily cratered surface mapping"));
        availableExperiments.push_back(ScienceData("Mercury", ExperimentType::GRAVITY_MEASUREMENT,
            12.0f, "Mercury core density analysis"));

        // Uranus & Neptune
        availableExperiments.push_back(ScienceData("Uranus", ExperimentType::VISUAL_SURVEY,
            28.0f, "Ice giant imaging"));
        availableExperiments.push_back(ScienceData("Neptune", ExperimentType::ATMOSPHERIC_SCAN,
            32.0f, "Extreme wind speed measurement"));

        // Black Hole experiments (high risk, high reward!)
        availableExperiments.push_back(ScienceData("Black Hole", ExperimentType::GRAVITY_MEASUREMENT,
            100.0f, "Event horizon gravitational data (EXTREME DANGER!)"));
        availableExperiments.push_back(ScienceData("Black Hole", ExperimentType::CLOSE_FLYBY,
            150.0f, "Accretion disk particle analysis (DO NOT GET TOO CLOSE!)"));

        // Sun experiments
        availableExperiments.push_back(ScienceData("Sun", ExperimentType::VISUAL_SURVEY,
            50.0f, "Solar corona and prominence imaging"));
        availableExperiments.push_back(ScienceData("Sun", ExperimentType::CLOSE_FLYBY,
            75.0f, "Solar wind particle measurement (HEAT WARNING!)"));
    }

    // Start scanning a target
    bool startScan(const std::string& targetName, float distance)
    {
        // Check if too far
        if (distance > 50.0f)
        {
            std::cout << "[SCIENCE] Target too far for scanning! Get within 50 units.\n";
            return false;
        }

        // Check if already scanned
        if (planetScanned[targetName])
        {
            std::cout << "[SCIENCE] " << targetName << " already fully scanned.\n";
            return false;
        }

        isScanning = true;
        scanProgress = 0.0f;
        currentScanTarget = targetName;

        std::cout << "\n╔═══════════════════════════════════════════════════════╗\n";
        std::cout << "║  🔬 INITIATING SCIENCE SCAN                           ║\n";
        std::cout << "╠═══════════════════════════════════════════════════════╣\n";
        std::cout << "║  Target: " << targetName;
        for (size_t i = targetName.length(); i < 44; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "║  Distance: " << distance << " units";
        for (size_t i = std::to_string(static_cast<int>(distance)).length() + 6; i < 42; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "║  Status: SCANNING...                                  ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════╝\n";

        return true;
    }

    // Update scan progress
    void updateScan(float deltaTime)
    {
        if (!isScanning) return;

        scanProgress += deltaTime / scanDuration;

        if (scanProgress >= 1.0f)
        {
            completeScan();
        }
    }

    // Complete the scan
    void completeScan()
    {
        if (!isScanning) return;

        isScanning = false;
        planetScanned[currentScanTarget] = true;

        // Award science points for all experiments on this planet
        float pointsEarned = 0.0f;
        int experimentsFound = 0;

        for (auto& exp : availableExperiments)
        {
            if (exp.planetName == currentScanTarget && !exp.collected)
            {
                exp.collected = true;
                pointsEarned += exp.scienceValue;
                experimentsFound++;
                experimentsCompleted++;

                std::cout << "  ✓ " << exp.description << " (+" << exp.scienceValue << " science)\n";
            }
        }

        totalSciencePoints += pointsEarned;

        std::cout << "\n╔═══════════════════════════════════════════════════════╗\n";
        std::cout << "║  🔬 SCAN COMPLETE!                                    ║\n";
        std::cout << "╠═══════════════════════════════════════════════════════╣\n";
        std::cout << "║  Target: " << currentScanTarget;
        for (size_t i = currentScanTarget.length(); i < 44; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "║  Experiments: " << experimentsFound;
        for (size_t i = std::to_string(experimentsFound).length(); i < 38; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "║  Science Earned: " << pointsEarned << " points";
        for (size_t i = std::to_string(static_cast<int>(pointsEarned)).length() + 7; i < 35; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "║  Total Science: " << totalSciencePoints << " points";
        for (size_t i = std::to_string(static_cast<int>(totalSciencePoints)).length() + 7; i < 36; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "╚═══════════════════════════════════════════════════════╝\n";
    }

    // Cancel scan (e.g., if moving too far)
    void cancelScan()
    {
        if (isScanning)
        {
            std::cout << "[SCIENCE] Scan interrupted! Distance too great or movement detected.\n";
            isScanning = false;
            scanProgress = 0.0f;
        }
    }

    // Get available experiments for a target
    std::vector<ScienceData> getAvailableExperiments(const std::string& targetName) const
    {
        std::vector<ScienceData> available;
        for (const auto& exp : availableExperiments)
        {
            if (exp.planetName == targetName && !exp.collected)
            {
                available.push_back(exp);
            }
        }
        return available;
    }

    // Display science summary
    void displaySummary() const
    {
        std::cout << "\n╔═══════════════════════════════════════════════════════╗\n";
        std::cout << "║           SCIENCE DATA SUMMARY                        ║\n";
        std::cout << "╠═══════════════════════════════════════════════════════╣\n";
        std::cout << "║  Total Science Points: " << totalSciencePoints;
        for (size_t i = std::to_string(static_cast<int>(totalSciencePoints)).length(); i < 29; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "║  Experiments Completed: " << experimentsCompleted << " / " << availableExperiments.size();
        for (size_t i = std::to_string(experimentsCompleted).length() + std::to_string(availableExperiments.size()).length() + 4; i < 27; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "║  Planets Scanned: " << planetScanned.size();
        for (size_t i = std::to_string(planetScanned.size()).length(); i < 34; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "╚═══════════════════════════════════════════════════════╝\n";
    }

    // Getters
    float getTotalSciencePoints() const { return totalSciencePoints; }
    int getExperimentsCompleted() const { return experimentsCompleted; }
    int getTotalExperiments() const { return static_cast<int>(availableExperiments.size()); }
    bool getIsScanning() const { return isScanning; }
    float getScanProgress() const { return scanProgress; }
    std::string getCurrentScanTarget() const { return currentScanTarget; }
};
