#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <ctime>
#include <filesystem>
#include <system_error>
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

    static std::string escapeString(const std::string& value)
    {
        std::string escaped;
        escaped.reserve(value.size());
        for (char character : value)
        {
            switch (character)
            {
                case '\\': escaped += "\\\\"; break;
                case '"': escaped += "\\\""; break;
                case '\n': escaped += "\\n"; break;
                case '\r': escaped += "\\r"; break;
                case '\t': escaped += "\\t"; break;
                default: escaped += character; break;
            }
        }
        return escaped;
    }

    static std::string unescapeString(const std::string& value)
    {
        std::string unescaped;
        unescaped.reserve(value.size());
        bool escaping = false;
        for (char character : value)
        {
            if (escaping)
            {
                switch (character)
                {
                    case 'n': unescaped += '\n'; break;
                    case 'r': unescaped += '\r'; break;
                    case 't': unescaped += '\t'; break;
                    default: unescaped += character; break;
                }
                escaping = false;
            }
            else if (character == '\\')
            {
                escaping = true;
            }
            else
            {
                unescaped += character;
            }
        }
        if (escaping) unescaped += '\\';
        return unescaped;
    }

    bool resolveSavePath(const std::string& filename, std::filesystem::path& result) const
    {
        const std::filesystem::path requested(filename);
        if (filename.empty() || requested.is_absolute() || requested.has_parent_path() ||
            requested.filename() == "." || requested.filename() == "..")
        {
            std::cerr << "[SaveSystem] ERROR: Invalid save filename: " << filename << "\n";
            return false;
        }

        result = std::filesystem::path(saveDirectory) / requested.filename();
        return true;
    }

    // Helper to write a key-value pair
    void writeField(std::ofstream& file, const std::string& key, const std::string& value)
    {
        file << "\"" << key << "\": \"" << escapeString(value) << "\",\n";
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

        const size_t valueStart = line.find_first_not_of(" \t", colonPos + 1);
        if (valueStart == std::string::npos) return "";
        if (line[valueStart] == '"')
        {
            std::string encoded;
            bool escaping = false;
            for (size_t i = valueStart + 1; i < line.size(); ++i)
            {
                const char character = line[i];
                if (character == '"' && !escaping) return unescapeString(encoded);
                encoded += character;
                escaping = character == '\\' && !escaping;
                if (character != '\\') escaping = false;
            }
            return "";
        }

        std::string value = line.substr(valueStart);
        value.erase(value.find_last_not_of(" \t,\r\n") + 1);
        return value;
    }

    static std::string extractKey(const std::string& line)
    {
        const size_t keyStart = line.find('"');
        if (keyStart == std::string::npos) return "";
        const size_t keyEnd = line.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) return "";
        return line.substr(keyStart + 1, keyEnd - keyStart - 1);
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
        std::error_code error;
        std::filesystem::create_directories(saveDirectory, error);
        if (error)
        {
            std::cerr << "[SaveSystem] ERROR: Cannot create save directory: "
                      << saveDirectory << " (" << error.message() << ")\n";
        }
    }

    // Save current game state to file
    bool save(const SaveData& data, const std::string& filename = "quicksave.sav")
    {
        std::filesystem::path savePath;
        if (!resolveSavePath(filename, savePath)) return false;
        const std::string fullPath = savePath.string();
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
        std::filesystem::path savePath;
        if (!resolveSavePath(filename, savePath)) return false;
        const std::string fullPath = savePath.string();
        std::ifstream file(fullPath);

        if (!file.is_open())
        {
            std::cerr << "[SaveSystem] ERROR: Cannot open file for reading: " << fullPath << "\n";
            return false;
        }

        const SaveData originalData = data;
        try
        {
            std::string line;
            while (std::getline(file, line))
            {
                const std::string key = extractKey(line);
                if (key == "shipPosition")
                    data.shipPosition = parseVec3(line);
            else if (key == "shipVelocity")
                data.shipVelocity = parseVec3(line);
            else if (key == "shipYaw")
                data.shipYaw = std::stof(extractValue(line));
            else if (key == "shipPitch")
                data.shipPitch = std::stof(extractValue(line));
            else if (key == "fuel")
                data.fuel = std::stof(extractValue(line));
            else if (key == "maxFuel")
                data.maxFuel = std::stof(extractValue(line));
            else if (key == "power")
                data.power = std::stof(extractValue(line));
            else if (key == "maxPower")
                data.maxPower = std::stof(extractValue(line));
            else if (key == "oxygen")
                data.oxygen = std::stof(extractValue(line));
            else if (key == "maxOxygen")
                data.maxOxygen = std::stof(extractValue(line));
            else if (key == "health")
                data.health = std::stof(extractValue(line));
            else if (key == "maxHealth")
                data.maxHealth = std::stof(extractValue(line));
            else if (key == "engineDamaged")
                data.engineDamaged = (extractValue(line) == "true");
            else if (key == "sensorsDamaged")
                data.sensorsDamaged = (extractValue(line) == "true");
            else if (key == "navigationDamaged")
                data.navigationDamaged = (extractValue(line) == "true");
            else if (key == "lifeSupportDamaged")
                data.lifeSupportDamaged = (extractValue(line) == "true");
            else if (key == "totalDistance")
                data.totalDistance = std::stof(extractValue(line));
            else if (key == "maxSpeed")
                data.maxSpeed = std::stof(extractValue(line));
            else if (key == "maxGForce")
                data.maxGForce = std::stof(extractValue(line));
            else if (key == "totalFlightTime")
                data.totalFlightTime = std::stof(extractValue(line));
            else if (key == "planetsVisited")
                data.planetsVisited = std::stoi(extractValue(line));
            else if (key == "achievementsUnlocked")
                data.achievementsUnlocked = std::stoi(extractValue(line));
            else if (key == "achievementPoints")
                data.achievementPoints = std::stoi(extractValue(line));
            else if (key == "sciencePoints")
                data.sciencePoints = std::stof(extractValue(line));
            else if (key == "experimentsCompleted")
                data.experimentsCompleted = std::stoi(extractValue(line));
            else if (key == "waypointsCreated")
                data.waypointsCreated = std::stoi(extractValue(line));
            else if (key == "closeCallsSurvived")
                data.closeCallsSurvived = std::stoi(extractValue(line));
            else if (key == "cosmicEventsSurvived")
                data.cosmicEventsSurvived = std::stoi(extractValue(line));
            else if (key == "landingAttempts")
                data.landingAttempts = std::stoi(extractValue(line));
            else if (key == "successfulLandings")
                data.successfulLandings = std::stoi(extractValue(line));
            else if (key == "totalGameTime")
                data.totalGameTime = std::stof(extractValue(line));
            else if (key == "timeScale")
                data.timeScale = std::stof(extractValue(line));
            else if (key == "currentTarget")
                data.currentTarget = extractValue(line);
            else if (key == "saveName")
                data.saveName = extractValue(line);
            else if (key == "saveDate")
                data.saveDate = extractValue(line);
                else if (key == "saveVersion")
                    data.saveVersion = std::stoi(extractValue(line));
            }
        }
        catch (const std::exception& error)
        {
            data = originalData;
            std::cerr << "[SaveSystem] ERROR: Invalid data in " << fullPath
                      << " (" << error.what() << ")\n";
            return false;
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
