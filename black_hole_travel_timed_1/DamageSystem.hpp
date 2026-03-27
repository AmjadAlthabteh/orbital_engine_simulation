#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>

// Subsystem types that can be damaged
enum class Subsystem
{
    ENGINES,
    SENSORS,
    NAVIGATION,
    LIFE_SUPPORT,
    COMMUNICATIONS,
    HULL
};

// Damage severity levels
enum class DamageSeverity
{
    NONE,
    MINOR,      // <25% functionality
    MODERATE,   // 25-50% functionality
    SEVERE,     // 50-75% functionality
    CRITICAL    // >75% functionality loss
};

// Individual subsystem damage state
struct SubsystemDamage
{
    Subsystem type;
    bool damaged;
    float damagePercent;        // 0-100%
    DamageSeverity severity;
    std::string statusMessage;

    SubsystemDamage(Subsystem t) : type(t), damaged(false), damagePercent(0.0f), severity(DamageSeverity::NONE)
    {
        statusMessage = "Operational";
    }

    void applyDamage(float amount)
    {
        damagePercent += amount;
        if (damagePercent > 100.0f) damagePercent = 100.0f;

        // Update severity
        if (damagePercent >= 75.0f)
        {
            severity = DamageSeverity::CRITICAL;
            damaged = true;
            statusMessage = "CRITICAL - System Failing";
        }
        else if (damagePercent >= 50.0f)
        {
            severity = DamageSeverity::SEVERE;
            damaged = true;
            statusMessage = "SEVERE - Major Malfunction";
        }
        else if (damagePercent >= 25.0f)
        {
            severity = DamageSeverity::MODERATE;
            damaged = true;
            statusMessage = "MODERATE - Performance Degraded";
        }
        else if (damagePercent > 0.0f)
        {
            severity = DamageSeverity::MINOR;
            damaged = true;
            statusMessage = "MINOR - Slight Degradation";
        }
        else
        {
            severity = DamageSeverity::NONE;
            damaged = false;
            statusMessage = "Operational";
        }
    }

    void repair(float amount)
    {
        damagePercent -= amount;
        if (damagePercent < 0.0f) damagePercent = 0.0f;
        applyDamage(0.0f);  // Recalculate severity
    }

    void fullRepair()
    {
        damagePercent = 0.0f;
        damaged = false;
        severity = DamageSeverity::NONE;
        statusMessage = "Operational";
    }

    float getFunctionalityPercent() const
    {
        return 100.0f - damagePercent;
    }
};

class DamageSystem
{
private:
    // Ship health
    float health;
    float maxHealth;

    // Hull integrity
    float hullIntegrity;  // 0-100%

    // Subsystems
    std::vector<SubsystemDamage> subsystems;

    // Damage tracking
    bool criticalDamageWarning;
    float timeSinceLastWarning;
    const float WARNING_COOLDOWN = 5.0f;

    // Damage thresholds
    const float CRITICAL_HEALTH_THRESHOLD = 25.0f;
    const float HULL_BREACH_THRESHOLD = 20.0f;

    SubsystemDamage* getSubsystem(Subsystem type)
    {
        for (auto& sys : subsystems)
        {
            if (sys.type == type)
                return &sys;
        }
        return nullptr;
    }

public:
    DamageSystem()
    {
        maxHealth = 100.0f;
        health = maxHealth;
        hullIntegrity = 100.0f;

        criticalDamageWarning = false;
        timeSinceLastWarning = 0.0f;

        // Initialize all subsystems
        subsystems.push_back(SubsystemDamage(Subsystem::ENGINES));
        subsystems.push_back(SubsystemDamage(Subsystem::SENSORS));
        subsystems.push_back(SubsystemDamage(Subsystem::NAVIGATION));
        subsystems.push_back(SubsystemDamage(Subsystem::LIFE_SUPPORT));
        subsystems.push_back(SubsystemDamage(Subsystem::COMMUNICATIONS));
        subsystems.push_back(SubsystemDamage(Subsystem::HULL));
    }

    // Update damage system
    void update(float deltaTime)
    {
        timeSinceLastWarning += deltaTime;

        // Check for critical damage
        if (health < CRITICAL_HEALTH_THRESHOLD && timeSinceLastWarning > WARNING_COOLDOWN)
        {
            if (!criticalDamageWarning)
            {
                criticalDamageWarning = true;
                std::cout << "\n[CRITICAL] SHIP HEALTH CRITICAL! Hull integrity failing!\n";
                timeSinceLastWarning = 0.0f;
            }
        }
        else if (health >= CRITICAL_HEALTH_THRESHOLD * 2.0f)
        {
            criticalDamageWarning = false;
        }

        // Hull breach warning
        if (hullIntegrity < HULL_BREACH_THRESHOLD && timeSinceLastWarning > WARNING_COOLDOWN)
        {
            std::cout << "\n[CRITICAL] HULL BREACH IMMINENT! " << hullIntegrity << "% integrity!\n";
            timeSinceLastWarning = 0.0f;
        }
    }

    // Take damage from various sources
    void takeDamage(float amount, const std::string& source = "Unknown")
    {
        health -= amount;
        if (health < 0.0f) health = 0.0f;

        // Hull integrity correlates with health
        hullIntegrity = (health / maxHealth) * 100.0f;

        std::cout << "\n[DamageSystem] DAMAGE TAKEN: " << amount << " from " << source << "\n";
        std::cout << "[DamageSystem] Health: " << health << "/" << maxHealth
                  << " (" << getHealthPercent() << "%)\n";

        // Random subsystem damage on major hits
        if (amount > 10.0f)
        {
            int randomSubsystem = rand() % subsystems.size();
            float subsystemDamage = amount * 0.5f;  // Subsystems take 50% of main damage
            subsystems[randomSubsystem].applyDamage(subsystemDamage);

            std::cout << "[DamageSystem] " << getSubsystemName(subsystems[randomSubsystem].type)
                      << " damaged! " << subsystems[randomSubsystem].statusMessage << "\n";
        }
    }

    // Specific damage sources
    void takeCollisionDamage(float impactVelocity)
    {
        float damage = impactVelocity * 2.0f;  // Higher velocity = more damage
        if (damage > 50.0f) damage = 50.0f;    // Cap collision damage
        takeDamage(damage, "Collision");
    }

    void takeGForceDamage(float gForce)
    {
        if (gForce > 10.0f)
        {
            float damage = (gForce - 10.0f) * 0.5f;  // 0.5 damage per G over 10
            takeDamage(damage, "Extreme G-Forces");
        }
    }

    void takeRadiationDamage(float radiationLevel, float deltaTime)
    {
        float damage = radiationLevel * deltaTime * 0.1f;
        takeDamage(damage, "Radiation");

        // Electronics more susceptible to radiation
        SubsystemDamage* sensors = getSubsystem(Subsystem::SENSORS);
        SubsystemDamage* nav = getSubsystem(Subsystem::NAVIGATION);
        if (sensors) sensors->applyDamage(damage * 0.5f);
        if (nav) nav->applyDamage(damage * 0.5f);
    }

    void takeSolarDamage(float solarIntensity, float deltaTime)
    {
        if (solarIntensity > 100.0f)
        {
            float damage = (solarIntensity - 100.0f) * deltaTime * 0.05f;
            takeDamage(damage, "Solar Radiation");
        }
    }

    // Damage specific subsystems
    void damageSubsystem(Subsystem type, float amount)
    {
        SubsystemDamage* sys = getSubsystem(type);
        if (sys)
        {
            sys->applyDamage(amount);
            std::cout << "[DamageSystem] " << getSubsystemName(type) << " damaged! "
                      << sys->statusMessage << "\n";
        }
    }

    // Repair functions
    void repair(float amount)
    {
        health += amount;
        if (health > maxHealth) health = maxHealth;
        hullIntegrity = (health / maxHealth) * 100.0f;

        std::cout << "[DamageSystem] Repairs complete! Health: " << health << "/" << maxHealth << "\n";
    }

    void repairSubsystem(Subsystem type, float amount)
    {
        SubsystemDamage* sys = getSubsystem(type);
        if (sys)
        {
            sys->repair(amount);
            std::cout << "[DamageSystem] " << getSubsystemName(type) << " repaired! "
                      << sys->getFunctionalityPercent() << "% functional\n";
        }
    }

    void fullRepair()
    {
        health = maxHealth;
        hullIntegrity = 100.0f;
        for (auto& sys : subsystems)
        {
            sys.fullRepair();
        }
        criticalDamageWarning = false;

        std::cout << "\n[DamageSystem] ===== FULL REPAIR COMPLETE =====\n";
        std::cout << "[DamageSystem] All systems restored to 100%!\n\n";
    }

    // Status checks
    bool isDestroyed() const { return health <= 0.0f; }
    bool isCritical() const { return health < CRITICAL_HEALTH_THRESHOLD; }
    bool hasHullBreach() const { return hullIntegrity < HULL_BREACH_THRESHOLD; }

    bool isSubsystemDamaged(Subsystem type) const
    {
        for (const auto& sys : subsystems)
        {
            if (sys.type == type)
                return sys.damaged;
        }
        return false;
    }

    float getSubsystemEfficiency(Subsystem type) const
    {
        for (const auto& sys : subsystems)
        {
            if (sys.type == type)
                return sys.getFunctionalityPercent() / 100.0f;  // Return 0-1
        }
        return 1.0f;  // If not found, assume operational
    }

    // Getters
    float getHealth() const { return health; }
    float getMaxHealth() const { return maxHealth; }
    float getHealthPercent() const { return (health / maxHealth) * 100.0f; }
    float getHullIntegrity() const { return hullIntegrity; }

    // Get subsystem name as string
    std::string getSubsystemName(Subsystem type) const
    {
        switch (type)
        {
            case Subsystem::ENGINES: return "Engines";
            case Subsystem::SENSORS: return "Sensors";
            case Subsystem::NAVIGATION: return "Navigation";
            case Subsystem::LIFE_SUPPORT: return "Life Support";
            case Subsystem::COMMUNICATIONS: return "Communications";
            case Subsystem::HULL: return "Hull";
            default: return "Unknown";
        }
    }

    // Display damage report
    void displayDamageReport() const
    {
        std::cout << "\n[DamageSystem] ========== DAMAGE REPORT ==========\n";
        std::cout << "[DamageSystem] Health: " << health << "/" << maxHealth
                  << " (" << getHealthPercent() << "%)\n";
        std::cout << "[DamageSystem] Hull Integrity: " << hullIntegrity << "%\n";
        std::cout << "[DamageSystem]\n";
        std::cout << "[DamageSystem] SUBSYSTEM STATUS:\n";

        for (const auto& sys : subsystems)
        {
            std::cout << "[DamageSystem]   " << getSubsystemName(sys.type) << ": "
                      << sys.getFunctionalityPercent() << "% - " << sys.statusMessage << "\n";
        }

        std::cout << "[DamageSystem] =====================================\n\n";
    }

    // Get damaged subsystems count
    int getDamagedSubsystemCount() const
    {
        int count = 0;
        for (const auto& sys : subsystems)
        {
            if (sys.damaged) count++;
        }
        return count;
    }

    // Get all subsystems (for UI display)
    const std::vector<SubsystemDamage>& getSubsystems() const { return subsystems; }

    // Setters (for save/load)
    void setHealth(float value) { health = (value > maxHealth) ? maxHealth : value; }
    void setMaxHealth(float value) { maxHealth = value; }
    void setHullIntegrity(float value) { hullIntegrity = value; }
    void setSubsystemDamage(Subsystem type, float damagePercent)
    {
        SubsystemDamage* sys = getSubsystem(type);
        if (sys)
        {
            sys->damagePercent = damagePercent;
            sys->applyDamage(0.0f);  // Recalculate severity
        }
    }
};
