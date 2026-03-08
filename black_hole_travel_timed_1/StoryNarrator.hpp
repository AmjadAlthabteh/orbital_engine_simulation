#pragma once
#include <string>
#include <vector>
#include <iostream>

// Story event types
enum class StoryEvent
{
    MISSION_START,
    FIRST_ORBIT,
    NEAR_SUN,
    PLANET_ENCOUNTER,
    BLACK_HOLE_WARNING,
    COLLISION_WARNING,
    ACHIEVEMENT,
    MILESTONE
};

// Narrative message with timing
struct NarrativeMessage
{
    std::string message;
    StoryEvent eventType;
    float displayTime;
    bool displayed;
};

// Story narrator system - provides contextual storytelling and narrative
class StoryNarrator
{
public:
    StoryNarrator();

    void update(float deltaTime);
    void triggerEvent(StoryEvent event, const std::string& customMessage = "");

    // Get current narrative message for HUD display
    std::string getCurrentMessage() const;
    bool hasActiveMessage() const;

private:
    void displayNarrative(const std::string& message, StoryEvent event);
    void checkMilestones(float totalTime);

    std::vector<NarrativeMessage> narrativeQueue;
    float messageTimer;
    float totalMissionTime;

    // Mission tracking
    bool hasEnteredOrbit;
    bool hasApproachedSun;
    bool hasSeenBlackHole;
    int planetsEncountered;
};
