#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#pragma once

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <ctime>

struct SessionStats
{
    // Flight statistics
    float totalDistance;
    float maxSpeed;
    float maxGForce;
    float totalFlightTime;
    int planetsVisited;
    int landingAttempts;
    int successfulLandings;

    // Science & Achievement
    int sciencePoints;
    int achievementPoints;
    int achievementsUnlocked;
    int experimentsCompleted;

    // Navigation
    int waypointsCreated;
    float closestBlackHoleApproach;
    float closestSunApproach;

    // Events & Survival
    int cosmicEventsExperienced;
    int closeCallsSurvived;
    int timesPaused;
    int timeWarpsUsed;

    SessionStats() : totalDistance(0), maxSpeed(0), maxGForce(0), totalFlightTime(0),
                     planetsVisited(0), landingAttempts(0), successfulLandings(0),
                     sciencePoints(0), achievementPoints(0), achievementsUnlocked(0),
                     experimentsCompleted(0), waypointsCreated(0),
                     closestBlackHoleApproach(999999), closestSunApproach(999999),
                     cosmicEventsExperienced(0), closeCallsSurvived(0),
                     timesPaused(0), timeWarpsUsed(0) {}
};

struct LifetimeStats
{
    int totalSessions;
    float totalDistanceAllTime;
    float maxSpeedAllTime;
    float maxGForceAllTime;
    float totalPlayTime;

    int totalPlanetsVisited;
    int totalLandingAttempts;
    int totalSuccessfulLandings;

    int totalSciencePoints;
    int totalAchievementPoints;
    int totalAchievements;
    int totalExperiments;

    int totalWaypoints;
    float closestBlackHoleAllTime;
    float closestSunAllTime;

    int totalCosmicEvents;
    int totalCloseCalls;

    LifetimeStats() : totalSessions(1), totalDistanceAllTime(0), maxSpeedAllTime(0),
                     maxGForceAllTime(0), totalPlayTime(0), totalPlanetsVisited(0),
                     totalLandingAttempts(0), totalSuccessfulLandings(0),
                     totalSciencePoints(0), totalAchievementPoints(0), totalAchievements(0),
                     totalExperiments(0), totalWaypoints(0),
                     closestBlackHoleAllTime(999999), closestSunAllTime(999999),
                     totalCosmicEvents(0), totalCloseCalls(0) {}
};

class EnhancedStatistics
{
private:
    SessionStats currentSession;
    LifetimeStats lifetime;

    // Rankings and records
    struct Record {
        std::string description;
        float value;
        std::string timestamp;
    };

    std::vector<Record> records;
    time_t sessionStartTime;

public:
    EnhancedStatistics()
    {
        sessionStartTime = time(0);
    }

    // Update methods
    void updateDistance(float distance) {
        currentSession.totalDistance += distance;
        lifetime.totalDistanceAllTime += distance;
    }

    void updateMaxSpeed(float speed) {
        if (speed > currentSession.maxSpeed) {
            currentSession.maxSpeed = speed;
            if (speed > lifetime.maxSpeedAllTime) {
                lifetime.maxSpeedAllTime = speed;
                addRecord("Fastest Speed", speed, " m/s");
            }
        }
    }

    void updateMaxGForce(float gForce) {
        if (gForce > currentSession.maxGForce) {
            currentSession.maxGForce = gForce;
            if (gForce > lifetime.maxGForceAllTime) {
                lifetime.maxGForceAllTime = gForce;
                addRecord("Maximum G-Force", gForce, " G");
            }
        }
    }

    void updateFlightTime(float deltaTime) {
        currentSession.totalFlightTime += deltaTime;
        lifetime.totalPlayTime += deltaTime;
    }

    void recordPlanetVisit() {
        currentSession.planetsVisited++;
        lifetime.totalPlanetsVisited++;
    }

    void recordLandingAttempt(bool successful) {
        currentSession.landingAttempts++;
        lifetime.totalLandingAttempts++;
        if (successful) {
            currentSession.successfulLandings++;
            lifetime.totalSuccessfulLandings++;
        }
    }

    void updateSciencePoints(int points) {
        currentSession.sciencePoints = points;
        lifetime.totalSciencePoints = std::max(lifetime.totalSciencePoints, points);
    }

    void updateAchievements(int points, int count) {
        currentSession.achievementPoints = points;
        currentSession.achievementsUnlocked = count;
        lifetime.totalAchievementPoints = std::max(lifetime.totalAchievementPoints, points);
        lifetime.totalAchievements = std::max(lifetime.totalAchievements, count);
    }

    void recordExperiment() {
        currentSession.experimentsCompleted++;
        lifetime.totalExperiments++;
    }

    void recordWaypoint() {
        currentSession.waypointsCreated++;
        lifetime.totalWaypoints++;
    }

    void updateBlackHoleDistance(float distance) {
        if (distance < currentSession.closestBlackHoleApproach) {
            currentSession.closestBlackHoleApproach = distance;
            if (distance < lifetime.closestBlackHoleAllTime) {
                lifetime.closestBlackHoleAllTime = distance;
                addRecord("Closest Black Hole Approach", distance, " units");
            }
        }
    }

    void updateSunDistance(float distance) {
        if (distance < currentSession.closestSunApproach) {
            currentSession.closestSunApproach = distance;
            if (distance < lifetime.closestSunAllTime) {
                lifetime.closestSunAllTime = distance;
                addRecord("Closest Sun Approach", distance, " units");
            }
        }
    }

    void recordCosmicEvent() {
        currentSession.cosmicEventsExperienced++;
        lifetime.totalCosmicEvents++;
    }

    void recordCloseCall() {
        currentSession.closeCallsSurvived++;
        lifetime.totalCloseCalls++;
    }

    void recordPause() {
        currentSession.timesPaused++;
    }

    void recordTimeWarp() {
        currentSession.timeWarpsUsed++;
    }

    // Display comprehensive statistics
    void displayComplete() const
    {
        std::cout << "\n╔═══════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                    📊 COMPLETE STATISTICS REPORT                      ║\n";
        std::cout << "╠═══════════════════════════════════════════════════════════════════════╣\n";

        // Session header
        time_t now = time(0);
        float sessionDuration = static_cast<float>(now - sessionStartTime);
        std::cout << "║  Current Session Duration: " << std::setw(6) << static_cast<int>(sessionDuration / 60)
                  << " minutes                                ║\n";
        std::cout << "║                                                                       ║\n";

        // Flight Statistics - Comparison
        std::cout << "║  ✈️  FLIGHT STATISTICS                                                 ║\n";
        std::cout << "║  ─────────────────────────────────────────────────────────────────────  ║\n";
        displayComparison("Total Distance", currentSession.totalDistance, lifetime.totalDistanceAllTime, " units");
        displayComparison("Max Speed", currentSession.maxSpeed, lifetime.maxSpeedAllTime, " m/s");
        displayComparison("Max G-Force", currentSession.maxGForce, lifetime.maxGForceAllTime, " G");
        displayComparison("Flight Time", currentSession.totalFlightTime / 60.0f, lifetime.totalPlayTime / 60.0f, " min");
        std::cout << "║                                                                       ║\n";

        // Exploration Statistics
        std::cout << "║  🌍 EXPLORATION & DISCOVERY                                            ║\n";
        std::cout << "║  ─────────────────────────────────────────────────────────────────────  ║\n";
        displayComparisonInt("Planets Visited", currentSession.planetsVisited, lifetime.totalPlanetsVisited);
        displayComparisonInt("Experiments Done", currentSession.experimentsCompleted, lifetime.totalExperiments);
        displayComparison("Science Points", currentSession.sciencePoints, lifetime.totalSciencePoints, "");
        displayComparisonInt("Waypoints Created", currentSession.waypointsCreated, lifetime.totalWaypoints);
        std::cout << "║                                                                       ║\n";

        // Achievement Progress
        std::cout << "║  🏆 ACHIEVEMENTS & PROGRESSION                                         ║\n";
        std::cout << "║  ─────────────────────────────────────────────────────────────────────  ║\n";
        displayComparisonInt("Achievements", currentSession.achievementsUnlocked, lifetime.totalAchievements);
        displayComparison("Achievement Pts", currentSession.achievementPoints, lifetime.totalAchievementPoints, "");

        float sessionCompletionRate = currentSession.landingAttempts > 0
            ? (static_cast<float>(currentSession.successfulLandings) / currentSession.landingAttempts * 100.0f)
            : 0.0f;
        float lifetimeCompletionRate = lifetime.totalLandingAttempts > 0
            ? (static_cast<float>(lifetime.totalSuccessfulLandings) / lifetime.totalLandingAttempts * 100.0f)
            : 0.0f;

        std::cout << "║  Landing Success: " << std::fixed << std::setprecision(1) << std::setw(5) << sessionCompletionRate
                  << "% (Session) | " << std::setw(5) << lifetimeCompletionRate << "% (Lifetime)            ║\n";
        std::cout << "║                                                                       ║\n";

        // Survival & Risk
        std::cout << "║  ⚠️  SURVIVAL & RISK MANAGEMENT                                        ║\n";
        std::cout << "║  ─────────────────────────────────────────────────────────────────────  ║\n";
        displayComparisonInt("Close Calls", currentSession.closeCallsSurvived, lifetime.totalCloseCalls);
        displayComparisonInt("Cosmic Events", currentSession.cosmicEventsExperienced, lifetime.totalCosmicEvents);

        if (currentSession.closestBlackHoleApproach < 999999) {
            displayComparison("Black Hole Approach", currentSession.closestBlackHoleApproach,
                            lifetime.closestBlackHoleAllTime, " units");
        }
        if (currentSession.closestSunApproach < 999999) {
            displayComparison("Sun Approach", currentSession.closestSunApproach,
                            lifetime.closestSunAllTime, " units");
        }
        std::cout << "║                                                                       ║\n";

        // Performance Metrics
        std::cout << "║  📈 PERFORMANCE METRICS                                                ║\n";
        std::cout << "║  ─────────────────────────────────────────────────────────────────────  ║\n";

        float avgSpeedSession = currentSession.totalFlightTime > 0
            ? currentSession.totalDistance / currentSession.totalFlightTime
            : 0.0f;
        float avgSpeedLifetime = lifetime.totalPlayTime > 0
            ? lifetime.totalDistanceAllTime / lifetime.totalPlayTime
            : 0.0f;
        displayComparison("Avg Speed", avgSpeedSession, avgSpeedLifetime, " m/s");

        float explorationRate = currentSession.totalFlightTime > 0
            ? currentSession.planetsVisited / (currentSession.totalFlightTime / 60.0f)
            : 0.0f;
        float explorationRateLifetime = lifetime.totalPlayTime > 0
            ? lifetime.totalPlanetsVisited / (lifetime.totalPlayTime / 60.0f)
            : 0.0f;
        displayComparison("Exploration Rate", explorationRate, explorationRateLifetime, " planets/min");

        std::cout << "╚═══════════════════════════════════════════════════════════════════════╝\n";

        // Display top records
        if (!records.empty()) {
            displayTopRecords();
        }
    }

    void displaySessionOnly() const
    {
        std::cout << "\n╔═══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║              📊 SESSION STATISTICS                            ║\n";
        std::cout << "╠═══════════════════════════════════════════════════════════════╣\n";
        std::cout << "║  Distance Traveled:  " << std::fixed << std::setprecision(1)
                  << std::setw(10) << currentSession.totalDistance << " units                     ║\n";
        std::cout << "║  Max Speed:          " << std::setw(10) << currentSession.maxSpeed << " m/s                       ║\n";
        std::cout << "║  Max G-Force:        " << std::setw(10) << currentSession.maxGForce << " G                         ║\n";
        std::cout << "║  Planets Visited:    " << std::setw(10) << currentSession.planetsVisited << "                              ║\n";
        std::cout << "║  Science Points:     " << std::setw(10) << currentSession.sciencePoints << "                              ║\n";
        std::cout << "║  Achievement Points: " << std::setw(10) << currentSession.achievementPoints << "                              ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════════╝\n";
    }

    // Getters
    const SessionStats& getSession() const { return currentSession; }
    const LifetimeStats& getLifetime() const { return lifetime; }

private:
    void addRecord(const std::string& desc, float value, const std::string& unit)
    {
        time_t now = time(0);
        tm timeInfo;
        localtime_s(&timeInfo, &now);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);

        Record rec;
        rec.description = desc;
        rec.value = value;
        rec.timestamp = std::string(buffer);

        records.push_back(rec);

        // Keep only last 10 records
        if (records.size() > 10) {
            records.erase(records.begin());
        }
    }

    void displayComparison(const std::string& label, float sessionVal, float lifetimeVal, const std::string& unit) const
    {
        std::cout << "║  " << std::left << std::setw(18) << label << std::right
                  << std::fixed << std::setprecision(1) << std::setw(8) << sessionVal << unit;

        // Pad to align the separator
        int padding = 10 - unit.length();
        for (int i = 0; i < padding; ++i) std::cout << " ";

        std::cout << "| " << std::setw(8) << lifetimeVal << unit;

        // Pad to end
        padding = 10 - unit.length();
        for (int i = 0; i < padding; ++i) std::cout << " ";
        std::cout << "║\n";
    }

    void displayComparisonInt(const std::string& label, int sessionVal, int lifetimeVal) const
    {
        std::cout << "║  " << std::left << std::setw(18) << label << std::right
                  << std::setw(8) << sessionVal << "          | "
                  << std::setw(8) << lifetimeVal << "          ║\n";
    }

    void displayTopRecords() const
    {
        std::cout << "\n╔═══════════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                        🏅 RECENT RECORDS                              ║\n";
        std::cout << "╠═══════════════════════════════════════════════════════════════════════╣\n";

        for (const auto& rec : records) {
            std::cout << "║  " << std::left << std::setw(30) << rec.description << std::right
                      << std::fixed << std::setprecision(2) << std::setw(10) << rec.value;
            std::cout << "  " << std::left << std::setw(25) << rec.timestamp << std::right << " ║\n";
        }

        std::cout << "╚═══════════════════════════════════════════════════════════════════════╝\n";
    }
};
