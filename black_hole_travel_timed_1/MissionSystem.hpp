#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <ctime>

// Mission types
enum class MissionType
{
    EXPLORATION,        // Visit specific locations
    SCIENCE,           // Complete science experiments
    SPEED_RUN,         // Reach location within time limit
    SURVIVAL,          // Survive specific conditions
    PRECISION,         // Precision landing or navigation
    COLLECTION,        // Collect multiple items/scans
    RESCUE             // Navigate to specific waypoint
};

// Mission difficulty
enum class MissionDifficulty
{
    EASY,
    MEDIUM,
    HARD,
    EXTREME
};

// Mission status
enum class MissionStatus
{
    LOCKED,            // Not yet available
    AVAILABLE,         // Can be started
    IN_PROGRESS,       // Currently active
    COMPLETED,         // Successfully finished
    FAILED             // Failed to complete
};

// Individual mission objective
struct MissionObjective
{
    std::string description;
    bool completed;
    float progress;        // 0-100%
    float requiredValue;   // Target value to reach
    float currentValue;    // Current progress value

    MissionObjective(const std::string& desc, float required = 1.0f)
        : description(desc), completed(false), progress(0.0f),
          requiredValue(required), currentValue(0.0f) {}

    void updateProgress(float value)
    {
        currentValue = value;
        if (currentValue >= requiredValue)
        {
            currentValue = requiredValue;
            completed = true;
            progress = 100.0f;
        }
        else
        {
            progress = (currentValue / requiredValue) * 100.0f;
        }
    }

    void complete()
    {
        completed = true;
        progress = 100.0f;
        currentValue = requiredValue;
    }
};

// Mission rewards
struct MissionRewards
{
    int credits;
    int sciencePoints;
    int achievementPoints;
    std::string unlockMessage;

    MissionRewards()
        : credits(0), sciencePoints(0), achievementPoints(0), unlockMessage("") {}

    MissionRewards(int c, int s, int a, const std::string& unlock = "")
        : credits(c), sciencePoints(s), achievementPoints(a), unlockMessage(unlock) {}
};

// Complete mission structure
struct Mission
{
    int id;
    std::string name;
    std::string description;
    MissionType type;
    MissionDifficulty difficulty;
    MissionStatus status;

    std::vector<MissionObjective> objectives;
    MissionRewards rewards;

    float timeLimit;         // In seconds (0 = no limit)
    float elapsedTime;       // Time spent on mission
    bool timeLimitActive;

    Mission(int missionId, const std::string& missionName, const std::string& desc,
            MissionType t, MissionDifficulty diff)
        : id(missionId), name(missionName), description(desc), type(t), difficulty(diff),
          status(MissionStatus::AVAILABLE), timeLimit(0.0f), elapsedTime(0.0f),
          timeLimitActive(false) {}

    bool isComplete() const
    {
        for (const auto& obj : objectives)
        {
            if (!obj.completed) return false;
        }
        return true;
    }

    float getOverallProgress() const
    {
        if (objectives.empty()) return 0.0f;

        float totalProgress = 0.0f;
        for (const auto& obj : objectives)
        {
            totalProgress += obj.progress;
        }
        return totalProgress / objectives.size();
    }

    bool isTimedOut() const
    {
        return timeLimitActive && (elapsedTime >= timeLimit);
    }
};

class MissionSystem
{
private:
    std::vector<Mission> missions;
    Mission* activeMission;
    int completedMissionCount;
    int totalCreditsEarned;

    void initializeMissions()
    {
        // Mission 1: First Steps
        Mission m1(1, "First Steps", "Learn the basics of space flight", MissionType::EXPLORATION, MissionDifficulty::EASY);
        m1.objectives.push_back(MissionObjective("Activate thrust", 1.0f));
        m1.objectives.push_back(MissionObjective("Reach 20 m/s speed", 1.0f));
        m1.objectives.push_back(MissionObjective("Create a waypoint", 1.0f));
        m1.rewards = MissionRewards(100, 10, 5, "Tutorial Complete!");
        missions.push_back(m1);

        // Mission 2: Planetary Explorer
        Mission m2(2, "Planetary Explorer", "Visit three different planets", MissionType::EXPLORATION, MissionDifficulty::EASY);
        m2.objectives.push_back(MissionObjective("Visit 3 planets", 3.0f));
        m2.rewards = MissionRewards(250, 30, 10);
        missions.push_back(m2);

        // Mission 3: Science Mission
        Mission m3(3, "Science Mission", "Complete 5 science experiments", MissionType::SCIENCE, MissionDifficulty::MEDIUM);
        m3.objectives.push_back(MissionObjective("Complete 5 experiments", 5.0f));
        m3.objectives.push_back(MissionObjective("Earn 100 science points", 100.0f));
        m3.rewards = MissionRewards(500, 50, 20, "Science Bay Upgrade Unlocked");
        missions.push_back(m3);

        // Mission 4: Speed Demon
        Mission m4(4, "Speed Demon", "Reach maximum velocity", MissionType::SPEED_RUN, MissionDifficulty::MEDIUM);
        m4.objectives.push_back(MissionObjective("Reach 100 m/s", 1.0f));
        m4.objectives.push_back(MissionObjective("Maintain speed for 10 seconds", 10.0f));
        m4.rewards = MissionRewards(400, 20, 15);
        missions.push_back(m4);

        // Mission 5: Precision Landing
        Mission m5(5, "Precision Landing", "Land safely on Mars", MissionType::PRECISION, MissionDifficulty::MEDIUM);
        m5.objectives.push_back(MissionObjective("Land on Mars", 1.0f));
        m5.objectives.push_back(MissionObjective("Landing velocity < 5 m/s", 1.0f));
        m5.rewards = MissionRewards(600, 40, 25);
        missions.push_back(m5);

        // Mission 6: Grand Tour
        Mission m6(6, "Grand Tour", "Visit all 8 planets", MissionType::EXPLORATION, MissionDifficulty::HARD);
        m6.objectives.push_back(MissionObjective("Visit all 8 planets", 8.0f));
        m6.objectives.push_back(MissionObjective("Complete 1 experiment per planet", 8.0f));
        m6.rewards = MissionRewards(2000, 200, 100, "Grand Tour Achievement");
        missions.push_back(m6);

        // Mission 7: Danger Zone
        Mission m7(7, "Danger Zone", "Survive extreme conditions", MissionType::SURVIVAL, MissionDifficulty::HARD);
        m7.objectives.push_back(MissionObjective("Survive 3 cosmic events", 3.0f));
        m7.objectives.push_back(MissionObjective("Survive 5 close calls", 5.0f));
        m7.objectives.push_back(MissionObjective("Approach black hole (< 15 units)", 1.0f));
        m7.rewards = MissionRewards(1500, 100, 75, "Reinforced Hull Upgrade");
        missions.push_back(m7);

        // Mission 8: Speed Challenge
        Mission m8(8, "Speed Challenge", "Reach Neptune in 10 minutes", MissionType::SPEED_RUN, MissionDifficulty::EXTREME);
        m8.objectives.push_back(MissionObjective("Reach Neptune", 1.0f));
        m8.timeLimit = 600.0f;  // 10 minutes
        m8.timeLimitActive = true;
        m8.rewards = MissionRewards(3000, 150, 150, "Elite Pilot Badge");
        missions.push_back(m8);

        // Mission 9: Science Master
        Mission m9(9, "Science Master", "Complete all science experiments", MissionType::SCIENCE, MissionDifficulty::EXTREME);
        m9.objectives.push_back(MissionObjective("Complete all experiments", 48.0f));  // 6 per planet * 8 planets
        m9.objectives.push_back(MissionObjective("Earn 1000 science points", 1000.0f));
        m9.rewards = MissionRewards(5000, 500, 200, "Master Scientist Title");
        missions.push_back(m9);

        // Mission 10: Perfect Run
        Mission m10(10, "Perfect Run", "Complete Grand Tour with no damage", MissionType::PRECISION, MissionDifficulty::EXTREME);
        m10.objectives.push_back(MissionObjective("Visit all 8 planets", 8.0f));
        m10.objectives.push_back(MissionObjective("Take no damage (100% health)", 1.0f));
        m10.objectives.push_back(MissionObjective("All landings under 3 m/s", 8.0f));
        m10.rewards = MissionRewards(10000, 1000, 500, "Perfect Pilot Achievement");
        missions.push_back(m10);

        std::cout << "[MissionSystem] " << missions.size() << " missions loaded\n";
    }

public:
    MissionSystem()
    {
        activeMission = nullptr;
        completedMissionCount = 0;
        totalCreditsEarned = 0;
        initializeMissions();
    }

    // Update active mission
    void update(float deltaTime)
    {
        if (activeMission && activeMission->status == MissionStatus::IN_PROGRESS)
        {
            if (activeMission->timeLimitActive)
            {
                activeMission->elapsedTime += deltaTime;

                // Check for timeout
                if (activeMission->isTimedOut())
                {
                    failMission();
                }
            }

            // Check for completion
            if (activeMission->isComplete())
            {
                completeMission();
            }
        }
    }

    // Start a mission
    bool startMission(int missionId)
    {
        Mission* mission = getMissionById(missionId);
        if (!mission)
        {
            std::cerr << "[MissionSystem] Mission not found: " << missionId << "\n";
            return false;
        }

        if (mission->status != MissionStatus::AVAILABLE)
        {
            std::cerr << "[MissionSystem] Mission not available: " << mission->name << "\n";
            return false;
        }

        if (activeMission)
        {
            std::cerr << "[MissionSystem] Another mission is already active\n";
            return false;
        }

        mission->status = MissionStatus::IN_PROGRESS;
        mission->elapsedTime = 0.0f;
        activeMission = mission;

        std::cout << "\n[MissionSystem] ========================================\n";
        std::cout << "[MissionSystem] MISSION STARTED: " << mission->name << "\n";
        std::cout << "[MissionSystem] " << mission->description << "\n";
        std::cout << "[MissionSystem] Difficulty: " << getDifficultyString(mission->difficulty) << "\n";
        if (mission->timeLimitActive)
        {
            std::cout << "[MissionSystem] Time Limit: " << (mission->timeLimit / 60.0f) << " minutes\n";
        }
        std::cout << "[MissionSystem] \nObjectives:\n";
        for (size_t i = 0; i < mission->objectives.size(); i++)
        {
            std::cout << "[MissionSystem]   " << (i+1) << ". " << mission->objectives[i].description << "\n";
        }
        std::cout << "[MissionSystem] ========================================\n\n";

        return true;
    }

    // Complete active mission
    void completeMission()
    {
        if (!activeMission) return;

        activeMission->status = MissionStatus::COMPLETED;
        completedMissionCount++;
        totalCreditsEarned += activeMission->rewards.credits;

        std::cout << "\n[MissionSystem] ==========================================\n";
        std::cout << "[MissionSystem] MISSION COMPLETE: " << activeMission->name << "\n";
        std::cout << "[MissionSystem] \nRewards:\n";
        std::cout << "[MissionSystem]   Credits: +" << activeMission->rewards.credits << "\n";
        std::cout << "[MissionSystem]   Science Points: +" << activeMission->rewards.sciencePoints << "\n";
        std::cout << "[MissionSystem]   Achievement Points: +" << activeMission->rewards.achievementPoints << "\n";
        if (!activeMission->rewards.unlockMessage.empty())
        {
            std::cout << "[MissionSystem]   " << activeMission->rewards.unlockMessage << "\n";
        }
        std::cout << "[MissionSystem] ==========================================\n\n";

        activeMission = nullptr;
    }

    // Fail active mission
    void failMission()
    {
        if (!activeMission) return;

        activeMission->status = MissionStatus::FAILED;

        std::cout << "\n[MissionSystem] ==========================================\n";
        std::cout << "[MissionSystem] MISSION FAILED: " << activeMission->name << "\n";
        if (activeMission->isTimedOut())
        {
            std::cout << "[MissionSystem] Reason: Time limit exceeded\n";
        }
        std::cout << "[MissionSystem] ==========================================\n\n";

        activeMission = nullptr;
    }

    // Update mission objective progress
    void updateObjectiveProgress(int missionId, int objectiveIndex, float value)
    {
        Mission* mission = getMissionById(missionId);
        if (mission && objectiveIndex < static_cast<int>(mission->objectives.size()))
        {
            mission->objectives[objectiveIndex].updateProgress(value);
        }
    }

    void completeObjective(int missionId, int objectiveIndex)
    {
        Mission* mission = getMissionById(missionId);
        if (mission && objectiveIndex < static_cast<int>(mission->objectives.size()))
        {
            mission->objectives[objectiveIndex].complete();
            std::cout << "[MissionSystem] Objective complete: "
                      << mission->objectives[objectiveIndex].description << "\n";
        }
    }

    // Get mission by ID
    Mission* getMissionById(int id)
    {
        for (auto& mission : missions)
        {
            if (mission.id == id) return &mission;
        }
        return nullptr;
    }

    // Display all missions
    void displayAllMissions() const
    {
        std::cout << "\n[MissionSystem] ========== AVAILABLE MISSIONS ==========\n";
        for (const auto& mission : missions)
        {
            std::cout << "[MissionSystem] [" << mission.id << "] " << mission.name
                      << " (" << getStatusString(mission.status) << ")\n";
            std::cout << "[MissionSystem]     " << mission.description << "\n";
            std::cout << "[MissionSystem]     Difficulty: " << getDifficultyString(mission.difficulty);
            if (mission.status == MissionStatus::IN_PROGRESS)
            {
                std::cout << " | Progress: " << mission.getOverallProgress() << "%";
            }
            std::cout << "\n";
        }
        std::cout << "[MissionSystem] =======================================\n\n";
    }

    // Display active mission progress
    void displayActiveMission() const
    {
        if (!activeMission)
        {
            std::cout << "[MissionSystem] No active mission\n";
            return;
        }

        std::cout << "\n[MissionSystem] ========== ACTIVE MISSION ==========\n";
        std::cout << "[MissionSystem] " << activeMission->name << "\n";
        std::cout << "[MissionSystem] Progress: " << activeMission->getOverallProgress() << "%\n";
        if (activeMission->timeLimitActive)
        {
            float remaining = activeMission->timeLimit - activeMission->elapsedTime;
            std::cout << "[MissionSystem] Time Remaining: " << (remaining / 60.0f) << " minutes\n";
        }
        std::cout << "[MissionSystem] \nObjectives:\n";
        for (const auto& obj : activeMission->objectives)
        {
            std::cout << "[MissionSystem]   [" << (obj.completed ? "X" : " ") << "] "
                      << obj.description << " (" << obj.progress << "%)\n";
        }
        std::cout << "[MissionSystem] ====================================\n\n";
    }

    // Helper functions
    std::string getStatusString(MissionStatus status) const
    {
        switch (status)
        {
            case MissionStatus::LOCKED: return "LOCKED";
            case MissionStatus::AVAILABLE: return "AVAILABLE";
            case MissionStatus::IN_PROGRESS: return "IN PROGRESS";
            case MissionStatus::COMPLETED: return "COMPLETED";
            case MissionStatus::FAILED: return "FAILED";
            default: return "UNKNOWN";
        }
    }

    std::string getDifficultyString(MissionDifficulty diff) const
    {
        switch (diff)
        {
            case MissionDifficulty::EASY: return "Easy";
            case MissionDifficulty::MEDIUM: return "Medium";
            case MissionDifficulty::HARD: return "Hard";
            case MissionDifficulty::EXTREME: return "Extreme";
            default: return "Unknown";
        }
    }

    // Getters
    Mission* getActiveMission() { return activeMission; }
    const std::vector<Mission>& getAllMissions() const { return missions; }
    int getCompletedMissionCount() const { return completedMissionCount; }
    int getTotalCreditsEarned() const { return totalCreditsEarned; }
};
