#include "StoryNarrator.hpp"

StoryNarrator::StoryNarrator()
    : messageTimer(0.0f), totalMissionTime(0.0f),
      hasEnteredOrbit(false), hasApproachedSun(false),
      hasSeenBlackHole(false), planetsEncountered(0)
{
    // Display opening narrative
    triggerEvent(StoryEvent::MISSION_START);
}

void StoryNarrator::update(float deltaTime)
{
    totalMissionTime += deltaTime;
    messageTimer += deltaTime;

    // Check for milestone-based narratives
    checkMilestones(totalMissionTime);

    // Remove old messages from queue
    for (auto it = narrativeQueue.begin(); it != narrativeQueue.end(); )
    {
        if (it->displayTime > 10.0f)  // Message expires after 10 seconds
        {
            it = narrativeQueue.erase(it);
        }
        else
        {
            it->displayTime += deltaTime;
            ++it;
        }
    }
}

void StoryNarrator::triggerEvent(StoryEvent event, const std::string& customMessage)
{
    std::string message;

    switch (event)
    {
        case StoryEvent::MISSION_START:
            message = R"(
╔══════════════════════════════════════════════════════════════════╗
║                   MISSION: BLACK HOLE EXPEDITION                  ║
╠══════════════════════════════════════════════════════════════════╣
║  Year 2384 - Deep Space Exploration Initiative                    ║
║                                                                    ║
║  You are Commander Alex Rivera, piloting the experimental         ║
║  spacecraft "Stellar Phoenix" through an uncharted star system.   ║
║                                                                    ║
║  MISSION OBJECTIVES:                                              ║
║  • Navigate through the solar system using gravity assists        ║
║  • Study planetary bodies and their orbital mechanics             ║
║  • Investigate the mysterious black hole at the system's edge     ║
║  • Return with valuable scientific data                           ║
║                                                                    ║
║  WARNING: The black hole's gravitational pull is immense.         ║
║  Approach with extreme caution. One mistake could be fatal.       ║
║                                                                    ║
║  Your ship is equipped with:                                      ║
║  - Advanced ion thrusters for precision maneuvering               ║
║  - Real-time collision prediction AI                              ║
║  - Trajectory analysis systems                                    ║
║  - Emergency escape protocols                                     ║
║                                                                    ║
║  "The cosmos is vast, Commander. Make history."                   ║
║  - Mission Control                                                ║
╚══════════════════════════════════════════════════════════════════╝
)";
            break;

        case StoryEvent::FIRST_ORBIT:
            if (!hasEnteredOrbit)
            {
                message = R"(
╔══════════════════════════════════════════════════════════════════╗
║  MISSION LOG: Stable Orbit Achieved                              ║
╠══════════════════════════════════════════════════════════════════╣
║  "Excellent work, Commander! Your ship has entered a stable      ║
║   orbital trajectory. You're now in perfect harmony with the     ║
║   gravitational forces at play."                                 ║
║                                                                   ║
║  This demonstrates your mastery of orbital mechanics. Remember:  ║
║  Every planet pulls on you differently. Use these forces wisely. ║
╚══════════════════════════════════════════════════════════════════╝
)";
                hasEnteredOrbit = true;
            }
            break;

        case StoryEvent::NEAR_SUN:
            if (!hasApproachedSun)
            {
                message = R"(
╔══════════════════════════════════════════════════════════════════╗
║  PROXIMITY ALERT: Approaching the Star                           ║
╠══════════════════════════════════════════════════════════════════╣
║  "Commander, you're getting dangerously close to the sun!        ║
║   Surface temperature: 5,778 Kelvin. Hull integrity: Stable."   ║
║                                                                   ║
║  SCIENCE NOTE: You're experiencing intense solar radiation.      ║
║  The sun's immense gravity (2000 mass units) dominates this      ║
║  region. Use a slingshot maneuver to gain velocity!              ║
║                                                                   ║
║  "The sun gives life... and takes it away." - Ancient Wisdom     ║
╚══════════════════════════════════════════════════════════════════╝
)";
                hasApproachedSun = true;
            }
            break;

        case StoryEvent::PLANET_ENCOUNTER:
            planetsEncountered++;
            if (planetsEncountered == 1)
            {
                message = R"(
╔══════════════════════════════════════════════════════════════════╗
║  DISCOVERY: Planetary Encounter                                  ║
╠══════════════════════════════════════════════════════════════════╣
║  "Fascinating! You're witnessing a planet in its natural orbit.  ║
║   Notice how it traces an elliptical path around the sun."       ║
║                                                                   ║
║  Each planet has unique characteristics:                         ║
║  • Mass affects gravitational pull on your ship                  ║
║  • Orbital velocity determines its path                          ║
║  • Atmospheric composition creates visual effects                ║
║                                                                   ║
║  DATA COLLECTED: Orbital parameters logged for analysis.         ║
╚══════════════════════════════════════════════════════════════════╝
)";
            }
            else if (planetsEncountered == 5)
            {
                message = R"(
╔══════════════════════════════════════════════════════════════════╗
║  ACHIEVEMENT UNLOCKED: Cosmic Explorer                           ║
╠══════════════════════════════════════════════════════════════════╣
║  You've encountered 5 different planetary bodies!                ║
║                                                                   ║
║  "The universe reveals its secrets to those who dare to look."   ║
╚══════════════════════════════════════════════════════════════════╝
)";
            }
            break;

        case StoryEvent::BLACK_HOLE_WARNING:
            if (!hasSeenBlackHole)
            {
                message = R"(
╔══════════════════════════════════════════════════════════════════╗
║  ⚠️  CRITICAL WARNING: BLACK HOLE DETECTED  ⚠️                     ║
╠══════════════════════════════════════════════════════════════════╣
║  "Commander, we're detecting massive gravitational distortions!  ║
║   The black hole's event horizon is within sensor range."        ║
║                                                                   ║
║  BLACK HOLE STATS:                                               ║
║  • Mass: 8000 units (4x larger than the sun!)                    ║
║  • Gravitational pull: EXTREME                                   ║
║  • Escape velocity: Near light speed                             ║
║                                                                   ║
║  WARNING: At 30 units distance, gravitational forces become      ║
║  overwhelming. Past the event horizon, there is no return.       ║
║                                                                   ║
║  "Stare into the abyss, and the abyss stares back."              ║
║  - Mission Commander's Journal                                   ║
╚══════════════════════════════════════════════════════════════════╝
)";
                hasSeenBlackHole = true;
            }
            break;

        case StoryEvent::COLLISION_WARNING:
            message = R"(
╔══════════════════════════════════════════════════════════════════╗
║  🚨 COLLISION ALERT! 🚨                                           ║
╠══════════════════════════════════════════════════════════════════╣
║  "PULL UP! PULL UP! Collision imminent!"                         ║
║                                                                   ║
║  Emergency thrusters activated. Take evasive action NOW!         ║
╚══════════════════════════════════════════════════════════════════╝
)";
            break;

        case StoryEvent::ACHIEVEMENT:
            message = customMessage.empty() ?
                "ACHIEVEMENT UNLOCKED!" : customMessage;
            break;

        case StoryEvent::MILESTONE:
            message = customMessage;
            break;
    }

    if (!message.empty())
    {
        displayNarrative(message, event);
    }
}

void StoryNarrator::displayNarrative(const std::string& message, StoryEvent event)
{
    // Print to console
    std::cout << "\n" << message << "\n" << std::endl;

    // Add to queue for HUD display
    NarrativeMessage narr;
    narr.message = message;
    narr.eventType = event;
    narr.displayTime = 0.0f;
    narr.displayed = true;

    narrativeQueue.push_back(narr);
}

void StoryNarrator::checkMilestones(float totalTime)
{
    static bool milestone30sec = false;
    static bool milestone1min = false;
    static bool milestone5min = false;

    if (totalTime > 30.0f && !milestone30sec)
    {
        milestone30sec = true;
        triggerEvent(StoryEvent::MILESTONE,
            R"(
╔══════════════════════════════════════════════════════════════════╗
║  MISSION TIME: 30 Seconds                                        ║
╠══════════════════════════════════════════════════════════════════╣
║  "You're adapting well to the ship's controls, Commander.        ║
║   Mission Control is impressed with your piloting skills."       ║
╚══════════════════════════════════════════════════════════════════╝
)");
    }

    if (totalTime > 60.0f && !milestone1min)
    {
        milestone1min = true;
        triggerEvent(StoryEvent::MILESTONE,
            R"(
╔══════════════════════════════════════════════════════════════════╗
║  MISSION TIME: 1 Minute                                          ║
╠══════════════════════════════════════════════════════════════════╣
║  "One minute into the mission. You're writing history.           ║
║   Every second of data is invaluable to humanity's understanding ║
║   of the cosmos."                                                ║
║                                                                   ║
║  Remember: This isn't just a simulation. These are the actual    ║
║  physics that govern our universe. Respect them.                 ║
╚══════════════════════════════════════════════════════════════════╝
)");
    }

    if (totalTime > 300.0f && !milestone5min)
    {
        milestone5min = true;
        triggerEvent(StoryEvent::MILESTONE,
            R"(
╔══════════════════════════════════════════════════════════════════╗
║  LEGENDARY STATUS: 5-Minute Survivor                             ║
╠══════════════════════════════════════════════════════════════════╣
║  "Incredible, Commander! You've survived 5 minutes in one of     ║
║   the most dangerous regions of space ever explored."            ║
║                                                                   ║
║  Your name will be remembered alongside the great explorers:     ║
║  • Armstrong  • Gagarin  • Rivera (YOU!)                         ║
║                                                                   ║
║  "Per aspera ad astra - Through hardships to the stars."         ║
╚══════════════════════════════════════════════════════════════════╝
)");
    }
}

std::string StoryNarrator::getCurrentMessage() const
{
    if (narrativeQueue.empty())
        return "";

    // Return most recent message if it's still fresh
    if (narrativeQueue.back().displayTime < 8.0f)
        return narrativeQueue.back().message;

    return "";
}

bool StoryNarrator::hasActiveMessage() const
{
    return !narrativeQueue.empty() && narrativeQueue.back().displayTime < 8.0f;
}
