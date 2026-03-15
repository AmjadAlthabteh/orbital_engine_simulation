#pragma once

#include <string>
#include <vector>
#include <random>
#include <iostream>
#include <ctime>

enum class EventType
{
    SOLAR_FLARE,
    METEOR_SHOWER,
    GRAVITATIONAL_ANOMALY,
    COSMIC_RAY_BURST,
    STELLAR_PHENOMENON,
    TIME_DILATION,
    MAGNETIC_STORM
};

struct CosmicEvent
{
    EventType type;
    std::string name;
    std::string description;
    float duration;          // How long event lasts (seconds)
    float intensity;         // 0.0 to 1.0
    bool active;
    float remainingTime;

    CosmicEvent(EventType t, const std::string& n, const std::string& desc, float dur, float intens)
        : type(t), name(n), description(desc), duration(dur),
          intensity(intens), active(false), remainingTime(0.0f)
    {}
};

class CosmicEventSystem
{
private:
    std::vector<CosmicEvent> possibleEvents;
    std::vector<CosmicEvent> activeEvents;
    std::mt19937 rng;
    float eventCheckTimer;
    float eventCheckInterval;  // Check for new events every X seconds

public:
    CosmicEventSystem() : eventCheckTimer(0.0f), eventCheckInterval(30.0f)
    {
        // Seed random number generator
        rng.seed(static_cast<unsigned int>(time(0)));

        initializeEvents();
    }

    void initializeEvents()
    {
        possibleEvents.push_back(CosmicEvent(
            EventType::SOLAR_FLARE,
            "Solar Flare Detected!",
            "Massive coronal mass ejection from the Sun. Radiation levels elevated.",
            15.0f, 0.7f
        ));

        possibleEvents.push_back(CosmicEvent(
            EventType::METEOR_SHOWER,
            "Meteor Shower Approaching",
            "Debris field detected! Micrometeorites may cause minor damage.",
            20.0f, 0.5f
        ));

        possibleEvents.push_back(CosmicEvent(
            EventType::GRAVITATIONAL_ANOMALY,
            "Gravitational Anomaly",
            "Unusual gravitational fluctuations detected. Navigation may be affected.",
            12.0f, 0.6f
        ));

        possibleEvents.push_back(CosmicEvent(
            EventType::COSMIC_RAY_BURST,
            "Cosmic Ray Burst",
            "High-energy particles detected from deep space source.",
            8.0f, 0.4f
        ));

        possibleEvents.push_back(CosmicEvent(
            EventType::STELLAR_PHENOMENON,
            "Stellar Phenomenon Observed",
            "Rare astronomical event occurring. Perfect for science observations!",
            25.0f, 0.8f
        ));

        possibleEvents.push_back(CosmicEvent(
            EventType::TIME_DILATION,
            "Time Dilation Effect",
            "Approaching massive object causing temporal distortion.",
            10.0f, 0.9f
        ));

        possibleEvents.push_back(CosmicEvent(
            EventType::MAGNETIC_STORM,
            "Magnetic Storm",
            "Intense magnetic field fluctuations. Compass navigation unreliable.",
            18.0f, 0.6f
        ));
    }

    void update(float deltaTime)
    {
        // Update active events
        for (auto it = activeEvents.begin(); it != activeEvents.end();)
        {
            it->remainingTime -= deltaTime;

            if (it->remainingTime <= 0.0f)
            {
                endEvent(*it);
                it = activeEvents.erase(it);
            }
            else
            {
                ++it;
            }
        }

        // Check for new events
        eventCheckTimer += deltaTime;
        if (eventCheckTimer >= eventCheckInterval)
        {
            eventCheckTimer = 0.0f;
            checkForNewEvent();
        }
    }

    void checkForNewEvent()
    {
        // Random chance to trigger an event (20% chance every check)
        std::uniform_real_distribution<float> dist(0.0f, 1.0f);
        float roll = dist(rng);

        if (roll < 0.2f && activeEvents.size() < 2)  // Max 2 concurrent events
        {
            // Pick a random event
            std::uniform_int_distribution<int> eventDist(0, static_cast<int>(possibleEvents.size()) - 1);
            int eventIndex = eventDist(rng);

            triggerEvent(possibleEvents[eventIndex]);
        }
    }

    void triggerEvent(const CosmicEvent& event)
    {
        // Check if this event type is already active
        for (const auto& active : activeEvents)
        {
            if (active.type == event.type)
                return;  // Don't trigger duplicate events
        }

        CosmicEvent newEvent = event;
        newEvent.active = true;
        newEvent.remainingTime = event.duration;
        activeEvents.push_back(newEvent);

        // Display event notification
        std::cout << "\n";
        std::cout << "╔═══════════════════════════════════════════════════════════╗\n";
        std::cout << "║  🌌 COSMIC EVENT!                                         ║\n";
        std::cout << "╠═══════════════════════════════════════════════════════════╣\n";
        std::cout << "║  " << event.name;
        for (size_t i = event.name.length(); i < 57; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "║  " << event.description;
        for (size_t i = event.description.length(); i < 57; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "║  Duration: " << static_cast<int>(event.duration) << " seconds";
        for (size_t i = std::to_string(static_cast<int>(event.duration)).length() + 11; i < 57; ++i) std::cout << " ";
        std::cout << "║\n";
        std::cout << "║  Intensity: ";

        // Visual intensity bar
        int bars = static_cast<int>(event.intensity * 20);
        for (int i = 0; i < bars; ++i) std::cout << "█";
        for (int i = bars; i < 20; ++i) std::cout << "░";

        std::cout << "                      ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
        std::cout << std::endl;
    }

    void endEvent(const CosmicEvent& event)
    {
        std::cout << "[COSMIC EVENT] " << event.name << " has ended.\n";
    }

    // Manual event trigger for testing
    void triggerEventByType(EventType type)
    {
        for (const auto& event : possibleEvents)
        {
            if (event.type == type)
            {
                triggerEvent(event);
                break;
            }
        }
    }

    // Get active events
    const std::vector<CosmicEvent>& getActiveEvents() const { return activeEvents; }

    // Check if specific event type is active
    bool isEventActive(EventType type) const
    {
        for (const auto& event : activeEvents)
        {
            if (event.type == type)
                return true;
        }
        return false;
    }

    // Get total intensity from all active events
    float getTotalIntensity() const
    {
        float total = 0.0f;
        for (const auto& event : activeEvents)
        {
            total += event.intensity;
        }
        return total;
    }
};
