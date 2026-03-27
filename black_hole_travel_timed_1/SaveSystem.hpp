#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <ctime>
#include "Vec3.hpp"

// Save/Load system for preserving game state
// Supports JSON-like format for easy debugging and editing

struct SaveData
{
    // Ship state
    Vec3 shipPosition;
    Vec3 shipVelocity;
    float shipYaw;
    float shipPitch;

    // Resources
    float fuel;
    float maxFuel;
    float power;
    float maxPower;
    float oxygen;
    float maxOxygen;

    // Health & Damage
    float health;
    float maxHealth;
    bool engineDamaged;
    bool sensorsDamaged;
    bool navigationDamaged;
    bool lifeSupportDamaged;

    // Statistics
    float totalDistance;
    float maxSpeed;
    float maxGForce;
    float totalFlightTime;
    int planetsVisited;
    int achievementsUnlocked;
    int achievementPoints;
    float sciencePoints;
    int experimentsCompleted;
    int waypointsCreated;
    int closeCallsSurvived;
    int cosmicEventsSurvived;
    int landingAttempts;
    int successfulLandings;

    // Game state
    float totalGameTime;
    float timeScale;
    std::string currentTarget;

    // Metadata
    std::string saveName;
    std::string saveDate;
    int saveVersion;

    SaveData()
    {
        // Default initialization
        shipPosition = Vec3(0, 0, 0);
        shipVelocity = Vec3(0, 0, 0);
        shipYaw = 0.0f;
        shipPitch = 0.0f;

        fuel = 1000.0f;
        maxFuel = 1000.0f;
        power = 100.0f;
        maxPower = 100.0f;
        oxygen = 100.0f;
        maxOxygen = 100.0f;

        health = 100.0f;
        maxHealth = 100.0f;
        engineDamaged = false;
        sensorsDamaged = false;
        navigationDamaged = false;
        lifeSupportDamaged = false;

        totalDistance = 0.0f;
        maxSpeed = 0.0f;
        maxGForce = 0.0f;
        totalFlightTime = 0.0f;
        planetsVisited = 0;
        achievementsUnlocked = 0;
        achievementPoints = 0;
        sciencePoints = 0.0f;
        experimentsCompleted = 0;
        waypointsCreated = 0;
        closeCallsSurvived = 0;
        cosmicEventsSurvived = 0;
        landingAttempts = 0;
        successfulLandings = 0;

        totalGameTime = 0.0f;
        timeScale = 1.0f;
        currentTarget = "None";

        saveName = "Quicksave";
        saveVersion = 1;
    }
};

class SaveSystem
{
private:
    std::string saveDirectory;
    std::string currentSaveFile;
    SaveData currentData;

    // Helper to write a key-value pair
    void writeField(std::ofstream& file, const std::string& key, const std::string& value)
    {
        file << "\"" << key << "\": \"" << value << "\",\n";
    }

    void writeField(std::ofstream& file, const std::string& key, float value)
    {
        file << "\"" << key << "\": " << value << ",\n";
    }

    void writeField(std::ofstream& file, const std::string& key, int value)
    {
        file << "\"" << key << "\": " << value << ",\n";
    }

    void writeField(std::ofstream& file, const std::string& key, bool value)
    {
        file << "\"" << key << "\": " << (value ? "true" : "false") << ",\n";
    }

    void writeVec3(std::ofstream& file, const std::string& key, const Vec3& vec)
    {
        file << "\"" << key << "\": [" << vec.x << ", " << vec.y << ", " << vec.z << "],\n";
    }

    // Helper to parse value from line
    std::string extractValue(const std::string& line)
    {
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) return "";

        std::string value = line.substr(colonPos + 1);
        // Remove quotes, spaces, commas
        value.erase(0, value.find_first_not_of(" \t\""));
        value.erase(value.find_last_not_of(" \t\",\n") + 1);
        return value;
    }

    Vec3 parseVec3(const std::string& value)
    {
        Vec3 result;
        size_t start = value.find('[');
        size_t end = value.find(']');
        if (start == std::string::npos || end == std::string::npos) return result;

        std::string content = value.substr(start + 1, end - start - 1);
        std::istringstream ss(content);
        char comma;
        ss >> result.x >> comma >> result.y >> comma >> result.z;
        return result;
    }

public:
    SaveSystem(const std::string& saveDir = "saves/")
        : saveDirectory(saveDir)
    {
        // Create save directory if it doesn't exist (platform-dependent)
        #ifdef _WIN32
        system(("mkdir " + saveDirectory + " 2>nul").c_str());
        #else
        system(("mkdir -p " + saveDirectory).c_str());
        #endif
    }

    // Save current game state to file
    bool save(const SaveData& data, const std::string& filename = "quicksave.sav")
    {
        std::string fullPath = saveDirectory + filename;
        std::ofstream file(fullPath);

        if (!file.is_open())
        {
            std::cerr << "[SaveSystem] ERROR: Cannot open file for writing: " << fullPath << "\n";
            return false;
        }

        currentData = data;
        currentSaveFile = fullPath;

        // Get current timestamp
        time_t now = time(0);
        char timeBuffer[26];
        #ifdef _WIN32
        ctime_s(timeBuffer, sizeof(timeBuffer), &now);
        #else
        strcpy(timeBuffer, ctime(&now));
        #endif

        // Write JSON-like format
        file << "{\n";
        writeField(file, "saveName", data.saveName);
        writeField(file, "saveDate", timeBuffer);
        writeField(file, "saveVersion", data.saveVersion);

        file << "\n// Ship State\n";
        writeVec3(file, "shipPosition", data.shipPosition);
        writeVec3(file, "shipVelocity", data.shipVelocity);
        writeField(file, "shipYaw", data.shipYaw);
        writeField(file, "shipPitch", data.shipPitch);

        file << "\n// Resources\n";
        writeField(file, "fuel", data.fuel);
        writeField(file, "maxFuel", data.maxFuel);
        writeField(file, "power", data.power);
        writeField(file, "maxPower", data.maxPower);
        writeField(file, "oxygen", data.oxygen);
        writeField(file, "maxOxygen", data.maxOxygen);

        file << "\n// Health & Damage\n";
        writeField(file, "health", data.health);
        writeField(file, "maxHealth", data.maxHealth);
        writeField(file, "engineDamaged", data.engineDamaged);
        writeField(file, "sensorsDamaged", data.sensorsDamaged);
        writeField(file, "navigationDamaged", data.navigationDamaged);
        writeField(file, "lifeSupportDamaged", data.lifeSupportDamaged);

        file << "\n// Statistics\n";
        writeField(file, "totalDistance", data.totalDistance);
        writeField(file, "maxSpeed", data.maxSpeed);
        writeField(file, "maxGForce", data.maxGForce);
        writeField(file, "totalFlightTime", data.totalFlightTime);
        writeField(file, "planetsVisited", data.planetsVisited);
        writeField(file, "achievementsUnlocked", data.achievementsUnlocked);
        writeField(file, "achievementPoints", data.achievementPoints);
        writeField(file, "sciencePoints", data.sciencePoints);
        writeField(file, "experimentsCompleted", data.experimentsCompleted);
        writeField(file, "waypointsCreated", data.waypointsCreated);
        writeField(file, "closeCallsSurvived", data.closeCallsSurvived);
        writeField(file, "cosmicEventsSurvived", data.cosmicEventsSurvived);
        writeField(file, "landingAttempts", data.landingAttempts);
        writeField(file, "successfulLandings", data.successfulLandings);

        file << "\n// Game State\n";
        writeField(file, "totalGameTime", data.totalGameTime);
        writeField(file, "timeScale", data.timeScale);
        writeField(file, "currentTarget", data.currentTarget);

        file << "\"end\": true\n";
        file << "}\n";

        file.close();

        std::cout << "\n[SaveSystem] ====================================\n";
        std::cout << "[SaveSystem] GAME SAVED SUCCESSFULLY\n";
        std::cout << "[SaveSystem] File: " << fullPath << "\n";
        std::cout << "[SaveSystem] Date: " << timeBuffer;
        std::cout << "[SaveSystem] ====================================\n\n";

        return true;
    }

    // Load game state from file
    bool load(SaveData& data, const std::string& filename = "quicksave.sav")
    {
        std::string fullPath = saveDirectory + filename;
        std::ifstream file(fullPath);

        if (!file.is_open())
        {
            std::cerr << "[SaveSystem] ERROR: Cannot open file for reading: " << fullPath << "\n";
            return false;
        }

        std::string line;
        while (std::getline(file, line))
        {
            if (line.find("shipPosition") != std::string::npos)
                data.shipPosition = parseVec3(line);
            else if (line.find("shipVelocity") != std::string::npos)
                data.shipVelocity = parseVec3(line);
            else if (line.find("shipYaw") != std::string::npos)
                data.shipYaw = std::stof(extractValue(line));
            else if (line.find("shipPitch") != std::string::npos)
                data.shipPitch = std::stof(extractValue(line));
            else if (line.find("fuel\"") != std::string::npos)
                data.fuel = std::stof(extractValue(line));
            else if (line.find("maxFuel") != std::string::npos)
                data.maxFuel = std::stof(extractValue(line));
            else if (line.find("power\"") != std::string::npos)
                data.power = std::stof(extractValue(line));
            else if (line.find("maxPower") != std::string::npos)
                data.maxPower = std::stof(extractValue(line));
            else if (line.find("oxygen\"") != std::string::npos)
                data.oxygen = std::stof(extractValue(line));
            else if (line.find("maxOxygen") != std::string::npos)
                data.maxOxygen = std::stof(extractValue(line));
            else if (line.find("health\"") != std::string::npos)
                data.health = std::stof(extractValue(line));
            else if (line.find("maxHealth") != std::string::npos)
                data.maxHealth = std::stof(extractValue(line));
            else if (line.find("engineDamaged") != std::string::npos)
                data.engineDamaged = (extractValue(line) == "true");
            else if (line.find("sensorsDamaged") != std::string::npos)
                data.sensorsDamaged = (extractValue(line) == "true");
            else if (line.find("navigationDamaged") != std::string::npos)
                data.navigationDamaged = (extractValue(line) == "true");
            else if (line.find("lifeSupportDamaged") != std::string::npos)
                data.lifeSupportDamaged = (extractValue(line) == "true");
            else if (line.find("totalDistance") != std::string::npos)
                data.totalDistance = std::stof(extractValue(line));
            else if (line.find("maxSpeed") != std::string::npos)
                data.maxSpeed = std::stof(extractValue(line));
            else if (line.find("maxGForce") != std::string::npos)
                data.maxGForce = std::stof(extractValue(line));
            else if (line.find("totalFlightTime") != std::string::npos)
                data.totalFlightTime = std::stof(extractValue(line));
            else if (line.find("planetsVisited") != std::string::npos)
                data.planetsVisited = std::stoi(extractValue(line));
            else if (line.find("achievementsUnlocked") != std::string::npos)
                data.achievementsUnlocked = std::stoi(extractValue(line));
            else if (line.find("achievementPoints") != std::string::npos)
                data.achievementPoints = std::stoi(extractValue(line));
            else if (line.find("sciencePoints") != std::string::npos)
                data.sciencePoints = std::stof(extractValue(line));
            else if (line.find("experimentsCompleted") != std::string::npos)
                data.experimentsCompleted = std::stoi(extractValue(line));
            else if (line.find("waypointsCreated") != std::string::npos)
                data.waypointsCreated = std::stoi(extractValue(line));
            else if (line.find("closeCallsSurvived") != std::string::npos)
                data.closeCallsSurvived = std::stoi(extractValue(line));
            else if (line.find("cosmicEventsSurvived") != std::string::npos)
                data.cosmicEventsSurvived = std::stoi(extractValue(line));
            else if (line.find("landingAttempts") != std::string::npos)
                data.landingAttempts = std::stoi(extractValue(line));
            else if (line.find("successfulLandings") != std::string::npos)
                data.successfulLandings = std::stoi(extractValue(line));
            else if (line.find("totalGameTime") != std::string::npos)
                data.totalGameTime = std::stof(extractValue(line));
            else if (line.find("timeScale") != std::string::npos)
                data.timeScale = std::stof(extractValue(line));
            else if (line.find("currentTarget") != std::string::npos)
                data.currentTarget = extractValue(line);
            else if (line.find("saveName") != std::string::npos)
                data.saveName = extractValue(line);
            else if (line.find("saveDate") != std::string::npos)
                data.saveDate = extractValue(line);
            else if (line.find("saveVersion") != std::string::npos)
                data.saveVersion = std::stoi(extractValue(line));
        }

        file.close();
        currentData = data;

        std::cout << "\n[SaveSystem] ====================================\n";
        std::cout << "[SaveSystem] GAME LOADED SUCCESSFULLY\n";
        std::cout << "[SaveSystem] File: " << fullPath << "\n";
        std::cout << "[SaveSystem] Save Date: " << data.saveDate;
        std::cout << "[SaveSystem] ====================================\n\n";

        return true;
    }

    // List all available save files
    std::vector<std::string> listSaves()
    {
        std::vector<std::string> saves;
        // Note: This is simplified - full implementation would scan directory
        saves.push_back("quicksave.sav");
        saves.push_back("autosave.sav");
        saves.push_back("manual_save_1.sav");
        return saves;
    }

    // Get current save data
    const SaveData& getCurrentData() const { return currentData; }

    // Quick save/load shortcuts
    bool quickSave(const SaveData& data)
    {
        return save(data, "quicksave.sav");
    }

    bool quickLoad(SaveData& data)
    {
        return load(data, "quicksave.sav");
    }

    bool autoSave(const SaveData& data)
    {
        return save(data, "autosave.sav");
    }
};
