#pragma once

#include "Vec3.hpp"
#include <string>
#include <vector>
#include <iostream>

struct Waypoint
{
    std::string name;
    Vec3 position;
    Vec3 color;
    float radius;
    bool active;
    std::string description;

    Waypoint(const std::string& n, const Vec3& pos, const Vec3& col = Vec3(1.0f, 1.0f, 0.0f))
        : name(n), position(pos), color(col), radius(2.0f), active(true), description("")
    {}
};

class WaypointSystem
{
private:
    std::vector<Waypoint> waypoints;
    int selectedWaypoint;

public:
    WaypointSystem() : selectedWaypoint(-1) {}

    // Add a waypoint at current ship position
    void addWaypoint(const std::string& name, const Vec3& position, const std::string& description = "")
    {
        Waypoint wp(name, position);
        wp.description = description;
        waypoints.push_back(wp);

        std::cout << "\n╔═══════════════════════════════════════════════════════╗\n";
        std::cout << "║  📍 WAYPOINT CREATED                                  ║\n";
        std::cout << "╠═══════════════════════════════════════════════════════╣\n";
        std::cout << "║  Name: " << name;
        for (size_t i = name.length(); i < 46; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "║  Position: (" << static_cast<int>(position.x) << ", "
                  << static_cast<int>(position.y) << ", "
                  << static_cast<int>(position.z) << ")";
        int spaces = 39 - std::to_string(static_cast<int>(position.x)).length()
                         - std::to_string(static_cast<int>(position.y)).length()
                         - std::to_string(static_cast<int>(position.z)).length();
        for (int i = 0; i < spaces; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "║  Total Waypoints: " << waypoints.size();
        for (size_t i = std::to_string(waypoints.size()).length(); i < 33; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "╚═══════════════════════════════════════════════════════╝\n";
    }

    // Remove a waypoint
    void removeWaypoint(int index)
    {
        if (index >= 0 && index < static_cast<int>(waypoints.size()))
        {
            std::cout << "[WAYPOINT] Removed: " << waypoints[index].name << "\n";
            waypoints.erase(waypoints.begin() + index);
            if (selectedWaypoint == index)
                selectedWaypoint = -1;
        }
    }

    // Select a waypoint for navigation
    void selectWaypoint(int index)
    {
        if (index >= 0 && index < static_cast<int>(waypoints.size()))
        {
            selectedWaypoint = index;
            std::cout << "[WAYPOINT] Selected: " << waypoints[index].name << "\n";
        }
    }

    // Get distance to selected waypoint
    float getDistanceToSelected(const Vec3& shipPos) const
    {
        if (selectedWaypoint >= 0 && selectedWaypoint < static_cast<int>(waypoints.size()))
        {
            Vec3 diff = waypoints[selectedWaypoint].position - shipPos;
            return diff.length();
        }
        return 0.0f;
    }

    // Get direction to selected waypoint
    Vec3 getDirectionToSelected(const Vec3& shipPos) const
    {
        if (selectedWaypoint >= 0 && selectedWaypoint < static_cast<int>(waypoints.size()))
        {
            Vec3 diff = waypoints[selectedWaypoint].position - shipPos;
            float len = diff.length();
            if (len > 0.001f)
                return Vec3(diff.x / len, diff.y / len, diff.z / len);
        }
        return Vec3(0, 0, 0);
    }

    // List all waypoints
    void listWaypoints() const
    {
        if (waypoints.empty())
        {
            std::cout << "[WAYPOINT] No waypoints created.\n";
            return;
        }

        std::cout << "\n╔═══════════════════════════════════════════════════════╗\n";
        std::cout << "║           WAYPOINT LIST                               ║\n";
        std::cout << "╠═══════════════════════════════════════════════════════╣\n";

        for (size_t i = 0; i < waypoints.size(); ++i)
        {
            const Waypoint& wp = waypoints[i];
            std::cout << "║  [" << i << "] " << wp.name;
            for (size_t j = wp.name.length() + std::to_string(i).length() + 4; j < 52; ++j)
                std::cout << " ";
            std::cout << "║\n";

            if (static_cast<int>(i) == selectedWaypoint)
            {
                std::cout << "║      ★ SELECTED                                       ║\n";
            }
        }

        std::cout << "╚═══════════════════════════════════════════════════════╝\n";
    }

    // Getters
    const std::vector<Waypoint>& getWaypoints() const { return waypoints; }
    int getSelectedWaypoint() const { return selectedWaypoint; }
    bool hasSelectedWaypoint() const { return selectedWaypoint >= 0 && selectedWaypoint < static_cast<int>(waypoints.size()); }

    // Quick waypoint presets
    void createPresetWaypoints(const std::vector<class CelestialBody*>& bodies)
    {
        // Create waypoints at interesting locations
        addWaypoint("Origin Point", Vec3(0, 0, 0), "The center of the solar system");
        addWaypoint("Safe Zone", Vec3(100, 0, 100), "Safe observation point");
        addWaypoint("Mission Start", Vec3(-20, 0, 50), "Default ship starting location");

        std::cout << "[WAYPOINT] Created 3 preset waypoints.\n";
    }
};
