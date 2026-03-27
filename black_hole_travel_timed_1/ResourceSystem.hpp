#pragma once

#include <string>
#include <iostream>
#include <algorithm>
#include "Vec3.hpp"
#include "CelestialBody.hpp"

// Resource types
enum class ResourceType
{
    FUEL,
    POWER,
    OXYGEN
};

// Resource consumption rates
struct ResourceRates
{
    float fuelConsumptionRate;      // Fuel per second when thrusting
    float idleFuelConsumption;      // Fuel per second when idle (life support, etc.)
    float powerDrainRate;           // Power per second (systems)
    float powerRegenRate;           // Power per second (solar panels)
    float oxygenConsumptionRate;    // Oxygen per second
    float oxygenRegenRate;          // Oxygen per second (when docked or landed)

    ResourceRates()
    {
        fuelConsumptionRate = 2.0f;     // 2 units/sec when thrusting
        idleFuelConsumption = 0.1f;     // 0.1 units/sec idle
        powerDrainRate = 0.5f;          // 0.5 units/sec
        powerRegenRate = 1.0f;          // 1 unit/sec near sun
        oxygenConsumptionRate = 0.2f;   // 0.2 units/sec
        oxygenRegenRate = 2.0f;         // 2 units/sec when landed
    }
};

class ResourceSystem
{
private:
    // Current resource levels
    float fuel;
    float maxFuel;
    float power;
    float maxPower;
    float oxygen;
    float maxOxygen;

    // Consumption rates
    ResourceRates rates;

    // State tracking
    bool isThrusting;
    bool isLanded;
    bool criticalFuelWarning;
    bool criticalPowerWarning;
    bool criticalOxygenWarning;

    // Solar power calculation
    float solarDistance;  // Distance to sun
    float optimalSolarDistance;  // Best distance for solar power

    // Warning thresholds
    const float CRITICAL_FUEL_THRESHOLD = 20.0f;
    const float CRITICAL_POWER_THRESHOLD = 15.0f;
    const float CRITICAL_OXYGEN_THRESHOLD = 25.0f;

public:
    ResourceSystem()
    {
        // Initialize with full resources
        maxFuel = 1000.0f;
        fuel = maxFuel;
        maxPower = 100.0f;
        power = maxPower;
        maxOxygen = 100.0f;
        oxygen = maxOxygen;

        isThrusting = false;
        isLanded = false;
        criticalFuelWarning = false;
        criticalPowerWarning = false;
        criticalOxygenWarning = false;

        solarDistance = 100.0f;
        optimalSolarDistance = 50.0f;  // Distance from sun for max solar efficiency
    }

    // Update resources based on time and ship state
    void update(float deltaTime, bool thrusting, bool landed, const Vec3& shipPos, const std::vector<CelestialBody*>& bodies)
    {
        isThrusting = thrusting;
        isLanded = landed;

        // Find sun for solar power calculation
        CelestialBody* sun = nullptr;
        for (auto* body : bodies)
        {
            if (body->getName() == "Sun")
            {
                sun = body;
                solarDistance = (shipPos - body->getPosition()).length();
                break;
            }
        }

        // === FUEL CONSUMPTION ===
        if (thrusting && fuel > 0.0f)
        {
            fuel -= rates.fuelConsumptionRate * deltaTime;
        }
        else if (!landed)
        {
            // Idle fuel consumption (life support, basic systems)
            fuel -= rates.idleFuelConsumption * deltaTime;
        }

        // Clamp fuel
        if (fuel < 0.0f) fuel = 0.0f;

        // === POWER MANAGEMENT ===
        // Solar power regeneration (stronger near sun)
        if (sun != nullptr && power < maxPower)
        {
            float solarEfficiency = calculateSolarEfficiency();
            power += rates.powerRegenRate * solarEfficiency * deltaTime;
        }

        // Power drain from systems
        if (!landed)
        {
            power -= rates.powerDrainRate * deltaTime;
        }

        // Clamp power
        if (power < 0.0f) power = 0.0f;
        if (power > maxPower) power = maxPower;

        // === OXYGEN CONSUMPTION ===
        if (oxygen > 0.0f)
        {
            oxygen -= rates.oxygenConsumptionRate * deltaTime;
        }

        // Oxygen regeneration when landed (breathing planets)
        if (landed)
        {
            // Check if landed on a planet with atmosphere
            for (auto* body : bodies)
            {
                float distToPlanet = (shipPos - body->getPosition()).length();
                if (distToPlanet < body->getRadius() + 2.0f && hasAtmosphere(body))
                {
                    oxygen += rates.oxygenRegenRate * deltaTime;
                    break;
                }
            }
        }

        // Clamp oxygen
        if (oxygen < 0.0f) oxygen = 0.0f;
        if (oxygen > maxOxygen) oxygen = maxOxygen;

        // === WARNING CHECKS ===
        checkWarnings();
    }

    // Calculate solar panel efficiency based on distance from sun
    float calculateSolarEfficiency() const
    {
        if (solarDistance <= 0.0f) return 0.0f;

        // Efficiency peaks at optimal distance, falls off with inverse square
        float efficiency = (optimalSolarDistance * optimalSolarDistance) / (solarDistance * solarDistance);

        // Cap efficiency between 0.1 and 2.0
        if (efficiency > 2.0f) efficiency = 2.0f;
        if (efficiency < 0.1f) efficiency = 0.1f;

        return efficiency;
    }

    // Check if planet has breathable atmosphere
    bool hasAtmosphere(CelestialBody* body)
    {
        std::string name = body->getName();
        return (name == "Earth" || name == "Venus" || name == "Mars" || name == "Jupiter" ||
                name == "Saturn" || name == "Uranus" || name == "Neptune");
    }

    // Refuel resources (at stations or planets)
    void refuel(float amount = -1.0f)
    {
        if (amount < 0.0f)
            fuel = maxFuel;  // Full refuel
        else
            fuel = std::min(fuel + amount, maxFuel);

        std::cout << "[ResourceSystem] Refueled! Fuel: " << fuel << "/" << maxFuel << "\n";
    }

    void rechargePower(float amount = -1.0f)
    {
        if (amount < 0.0f)
            power = maxPower;
        else
            power = std::min(power + amount, maxPower);

        std::cout << "[ResourceSystem] Power recharged! Power: " << power << "/" << maxPower << "\n";
    }

    void replenishOxygen(float amount = -1.0f)
    {
        if (amount < 0.0f)
            oxygen = maxOxygen;
        else
            oxygen = std::min(oxygen + amount, maxOxygen);

        std::cout << "[ResourceSystem] Oxygen replenished! Oxygen: " << oxygen << "/" << maxOxygen << "\n";
    }

    // Full resupply (when landing on suitable planet)
    void fullResupply()
    {
        refuel();
        rechargePower();
        replenishOxygen();
        std::cout << "\n[ResourceSystem] ===== FULL RESUPPLY COMPLETE =====\n";
        std::cout << "[ResourceSystem] All resources restored to maximum!\n\n";
    }

    // Check and display warnings
    void checkWarnings()
    {
        // Fuel warning
        if (fuel < CRITICAL_FUEL_THRESHOLD && !criticalFuelWarning)
        {
            criticalFuelWarning = true;
            std::cout << "\n[WARNING] FUEL CRITICAL! Only " << fuel << " units remaining!\n";
        }
        else if (fuel >= CRITICAL_FUEL_THRESHOLD * 2.0f)
        {
            criticalFuelWarning = false;
        }

        // Power warning
        if (power < CRITICAL_POWER_THRESHOLD && !criticalPowerWarning)
        {
            criticalPowerWarning = true;
            std::cout << "\n[WARNING] POWER CRITICAL! Systems failing!\n";
        }
        else if (power >= CRITICAL_POWER_THRESHOLD * 2.0f)
        {
            criticalPowerWarning = false;
        }

        // Oxygen warning
        if (oxygen < CRITICAL_OXYGEN_THRESHOLD && !criticalOxygenWarning)
        {
            criticalOxygenWarning = true;
            std::cout << "\n[WARNING] OXYGEN CRITICAL! Life support failing!\n";
        }
        else if (oxygen >= CRITICAL_OXYGEN_THRESHOLD * 2.0f)
        {
            criticalOxygenWarning = false;
        }
    }

    // Upgrade resource capacities
    void upgradeFuelCapacity(float amount)
    {
        maxFuel += amount;
        std::cout << "[ResourceSystem] Fuel capacity upgraded! New max: " << maxFuel << "\n";
    }

    void upgradePowerCapacity(float amount)
    {
        maxPower += amount;
        std::cout << "[ResourceSystem] Power capacity upgraded! New max: " << maxPower << "\n";
    }

    void upgradeOxygenCapacity(float amount)
    {
        maxOxygen += amount;
        std::cout << "[ResourceSystem] Oxygen capacity upgraded! New max: " << maxOxygen << "\n";
    }

    // Check if ship can thrust (needs fuel and power)
    bool canThrust() const
    {
        return (fuel > 1.0f && power > 5.0f);
    }

    // Check critical states
    bool isFuelCritical() const { return fuel < CRITICAL_FUEL_THRESHOLD; }
    bool isPowerCritical() const { return power < CRITICAL_POWER_THRESHOLD; }
    bool isOxygenCritical() const { return oxygen < CRITICAL_OXYGEN_THRESHOLD; }
    bool isAnyCritical() const { return isFuelCritical() || isPowerCritical() || isOxygenCritical(); }

    // Getters
    float getFuel() const { return fuel; }
    float getMaxFuel() const { return maxFuel; }
    float getFuelPercent() const { return (fuel / maxFuel) * 100.0f; }

    float getPower() const { return power; }
    float getMaxPower() const { return maxPower; }
    float getPowerPercent() const { return (power / maxPower) * 100.0f; }

    float getOxygen() const { return oxygen; }
    float getMaxOxygen() const { return maxOxygen; }
    float getOxygenPercent() const { return (oxygen / maxOxygen) * 100.0f; }

    float getSolarEfficiency() const { return calculateSolarEfficiency(); }
    float getSolarDistance() const { return solarDistance; }

    // Display resource status
    void displayStatus() const
    {
        std::cout << "\n[ResourceSystem] ========== RESOURCE STATUS ==========\n";
        std::cout << "[ResourceSystem] FUEL:   " << fuel << "/" << maxFuel
                  << " (" << getFuelPercent() << "%)"
                  << (isFuelCritical() ? " [CRITICAL]" : "") << "\n";
        std::cout << "[ResourceSystem] POWER:  " << power << "/" << maxPower
                  << " (" << getPowerPercent() << "%)"
                  << (isPowerCritical() ? " [CRITICAL]" : "") << "\n";
        std::cout << "[ResourceSystem] OXYGEN: " << oxygen << "/" << maxOxygen
                  << " (" << getOxygenPercent() << "%)"
                  << (isOxygenCritical() ? " [CRITICAL]" : "") << "\n";
        std::cout << "[ResourceSystem] Solar Efficiency: " << (calculateSolarEfficiency() * 100.0f) << "%\n";
        std::cout << "[ResourceSystem] =======================================\n\n";
    }

    // Setters (for save/load)
    void setFuel(float value) { fuel = std::min(value, maxFuel); }
    void setPower(float value) { power = std::min(value, maxPower); }
    void setOxygen(float value) { oxygen = std::min(value, maxOxygen); }
    void setMaxFuel(float value) { maxFuel = value; }
    void setMaxPower(float value) { maxPower = value; }
    void setMaxOxygen(float value) { maxOxygen = value; }

    // Get consumption rates
    const ResourceRates& getRates() const { return rates; }
    void setRates(const ResourceRates& newRates) { rates = newRates; }
};
