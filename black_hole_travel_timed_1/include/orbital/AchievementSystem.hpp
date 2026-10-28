#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <ctime>

// Achievement types
enum class AchievementType
{
    SPEED,          // Speed milestones
    DISTANCE,       // Distance traveled
    EXPLORATION,    // Planets visited
    SURVIVAL,       // Near-death experiences
    MASTERY,        // Skill demonstrations
    SECRET          // Hidden achievements
};

struct Achievement
{
    std::string id;
    std::string name;
    std::string description;
    AchievementType type;
    bool unlocked;
    float progress;        // 0.0 to 1.0
    float requirement;     // What value is needed
    int points;           // Achievement points awarded
    std::string unlockedTime;

    Achievement(const std::string& _id, const std::string& _name, const std::string& _desc,
                AchievementType _type, float _req, int _points)
        : id(_id), name(_name), description(_desc), type(_type),
          unlocked(false), progress(0.0f), requirement(_req), points(_points),
          unlockedTime("")
    {}
};

class AchievementSystem
{
private:
    std::vector<Achievement> achievements;
    int totalPoints;

    // Tracking variables
    float maxSpeed;
    float totalDistance;
    int planetsVisited;
    int closeCallsCount;
    bool hasLanded;
    bool hasUsedHohmann;
    bool hasSeenBlackHole;

public:
    AchievementSystem() : totalPoints(0), maxSpeed(0.0f), totalDistance(0.0f),
                         planetsVisited(0), closeCallsCount(0), hasLanded(false),
                         hasUsedHohmann(false), hasSeenBlackHole(false)
    {
        initializeAchievements();
    }

    void initializeAchievements()
    {
        // Speed achievements
        achievements.push_back(Achievement("speed_novice", "Space Cadet",
            "Reach 20 m/s", AchievementType::SPEED, 20.0f, 10));
        achievements.push_back(Achievement("speed_pilot", "Cosmic Pilot",
            "Reach 50 m/s", AchievementType::SPEED, 50.0f, 25));
        achievements.push_back(Achievement("speed_ace", "Speed Demon",
            "Reach 100 m/s", AchievementType::SPEED, 100.0f, 50));
        achievements.push_back(Achievement("speed_legend", "Lightspeed Legend",
            "Reach 150 m/s", AchievementType::SPEED, 150.0f, 100));

        // Exploration achievements
        achievements.push_back(Achievement("explore_first", "First Contact",
            "Visit your first planet", AchievementType::EXPLORATION, 1.0f, 15));
        achievements.push_back(Achievement("explore_tourist", "Space Tourist",
            "Visit 3 different planets", AchievementType::EXPLORATION, 3.0f, 30));
        achievements.push_back(Achievement("explore_voyager", "Cosmic Voyager",
            "Visit 5 different planets", AchievementType::EXPLORATION, 5.0f, 60));
        achievements.push_back(Achievement("explore_all", "Grand Tour",
            "Visit all 8 planets", AchievementType::EXPLORATION, 8.0f, 150));

        // Survival achievements
        achievements.push_back(Achievement("survive_close", "Close Call",
            "Survive altitude below 5 units", AchievementType::SURVIVAL, 1.0f, 20));
        achievements.push_back(Achievement("survive_daredevil", "Daredevil",
            "Survive 5 close calls", AchievementType::SURVIVAL, 5.0f, 50));
        achievements.push_back(Achievement("survive_blackhole", "Event Horizon Survivor",
            "Approach black hole within 15 units", AchievementType::SURVIVAL, 1.0f, 75));

        // Mastery achievements
        achievements.push_back(Achievement("master_landing", "Smooth Landing",
            "Successfully land on a planet", AchievementType::MASTERY, 1.0f, 40));
        achievements.push_back(Achievement("master_hohmann", "Orbital Mechanic",
            "Calculate a Hohmann transfer", AchievementType::MASTERY, 1.0f, 35));
        achievements.push_back(Achievement("master_precision", "Precision Pilot",
            "Match velocity with target (within 5 m/s)", AchievementType::MASTERY, 1.0f, 45));

        // Distance achievements
        achievements.push_back(Achievement("distance_explorer", "Star Wanderer",
            "Travel 500 units total", AchievementType::DISTANCE, 500.0f, 25));
        achievements.push_back(Achievement("distance_voyager", "Deep Space Voyager",
            "Travel 2000 units total", AchievementType::DISTANCE, 2000.0f, 75));
        achievements.push_back(Achievement("distance_legend", "Interstellar Legend",
            "Travel 5000 units total", AchievementType::DISTANCE, 5000.0f, 150));

        // Secret achievements
        achievements.push_back(Achievement("secret_sun", "Too Close to the Sun",
            "Get within 10 units of the Sun", AchievementType::SECRET, 1.0f, 50));
        achievements.push_back(Achievement("secret_slowpoke", "Zen Master",
            "Travel at exactly 0 m/s for 5 seconds", AchievementType::SECRET, 1.0f, 30));
    }

    // Update tracking stats
    void updateSpeed(float speed)
    {
        if (speed > maxSpeed)
        {
            maxSpeed = speed;
            checkSpeedAchievements();
        }
    }

    void updateDistance(float distance)
    {
        totalDistance += distance;
        checkDistanceAchievements();
    }

    void recordPlanetVisit(const std::string& planetName)
    {
        planetsVisited++;
        checkExplorationAchievements();
    }

    void recordCloseCall()
    {
        closeCallsCount++;
        unlockAchievement("survive_close");

        if (closeCallsCount >= 5)
            unlockAchievement("survive_daredevil");
    }

    void recordBlackHoleApproach()
    {
        if (!hasSeenBlackHole)
        {
            hasSeenBlackHole = true;
            unlockAchievement("survive_blackhole");
        }
    }

    void recordLanding()
    {
        if (!hasLanded)
        {
            hasLanded = true;
            unlockAchievement("master_landing");
        }
    }

    void recordHohmannTransfer()
    {
        if (!hasUsedHohmann)
        {
            hasUsedHohmann = true;
            unlockAchievement("master_hohmann");
        }
    }

    void recordSunApproach()
    {
        unlockAchievement("secret_sun");
    }

    void checkSpeedAchievements()
    {
        for (auto& ach : achievements)
        {
            if (ach.type == AchievementType::SPEED && !ach.unlocked)
            {
                ach.progress = maxSpeed / ach.requirement;
                if (maxSpeed >= ach.requirement)
                {
                    unlockAchievement(ach.id);
                }
            }
        }
    }

    void checkDistanceAchievements()
    {
        for (auto& ach : achievements)
        {
            if (ach.type == AchievementType::DISTANCE && !ach.unlocked)
            {
                ach.progress = totalDistance / ach.requirement;
                if (totalDistance >= ach.requirement)
                {
                    unlockAchievement(ach.id);
                }
            }
        }
    }

    void checkExplorationAchievements()
    {
        for (auto& ach : achievements)
        {
            if (ach.type == AchievementType::EXPLORATION && !ach.unlocked)
            {
                ach.progress = static_cast<float>(planetsVisited) / ach.requirement;
                if (planetsVisited >= static_cast<int>(ach.requirement))
                {
                    unlockAchievement(ach.id);
                }
            }
        }
    }

    void unlockAchievement(const std::string& id)
    {
        for (auto& ach : achievements)
        {
            if (ach.id == id && !ach.unlocked)
            {
                ach.unlocked = true;
                ach.progress = 1.0f;
                totalPoints += ach.points;

                // Get current time
                time_t now = time(0);
                char timeBuffer[26];
                ctime_s(timeBuffer, sizeof(timeBuffer), &now);
                ach.unlockedTime = timeBuffer;

                // Display unlock notification
                std::cout << "\n";
                std::cout << "╔═══════════════════════════════════════════════════════╗\n";
                std::cout << "║          🏆 ACHIEVEMENT UNLOCKED! 🏆                  ║\n";
                std::cout << "╠═══════════════════════════════════════════════════════╣\n";
                std::cout << "║  " << ach.name;
                // Pad to 50 chars
                for (size_t i = ach.name.length(); i < 50; ++i) std::cout << " ";
                std::cout << "║\n";
                std::cout << "║  " << ach.description;
                for (size_t i = ach.description.length(); i < 50; ++i) std::cout << " ";
                std::cout << "║\n";
                std::cout << "║  +" << ach.points << " Achievement Points";
                int spaces = 50 - 20 - std::to_string(ach.points).length();
                for (int i = 0; i < spaces; ++i) std::cout << " ";
                std::cout << "║\n";
                std::cout << "╚═══════════════════════════════════════════════════════╝\n";
                std::cout << std::endl;
            }
        }
    }

    // Get stats
    int getTotalPoints() const { return totalPoints; }
    int getUnlockedCount() const
    {
        int count = 0;
        for (const auto& ach : achievements)
            if (ach.unlocked) count++;
        return count;
    }
    int getTotalCount() const { return static_cast<int>(achievements.size()); }
    const std::vector<Achievement>& getAchievements() const { return achievements; }

    // Display summary
    void displaySummary() const
    {
        std::cout << "\n╔═══════════════════════════════════════════════════════╗\n";
        std::cout << "║           ACHIEVEMENT PROGRESS SUMMARY                ║\n";
        std::cout << "╠═══════════════════════════════════════════════════════╣\n";
        std::cout << "║  Total Points: " << totalPoints;
        for (size_t i = std::to_string(totalPoints).length(); i < 37; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "║  Unlocked: " << getUnlockedCount() << " / " << getTotalCount();
        for (size_t i = std::to_string(getUnlockedCount()).length() + std::to_string(getTotalCount()).length() + 4; i < 41; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "║  Max Speed: " << maxSpeed << " m/s";
        for (size_t i = std::to_string(static_cast<int>(maxSpeed)).length() + 4; i < 38; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "║  Planets Visited: " << planetsVisited;
        for (size_t i = std::to_string(planetsVisited).length(); i < 34; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "╚═══════════════════════════════════════════════════════╝\n";
    }

    float getMaxSpeed() const { return maxSpeed; }
    float getTotalDistance() const { return totalDistance; }
    int getPlanetsVisited() const { return planetsVisited; }
};
