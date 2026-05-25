#pragma once

#include "imgui.h"
#include "CelestialBody.hpp"
#include "ScienceSystem.hpp"
#include "Body.hpp"
#include <string>
#include <map>
#include <vector>

class PlanetInformationPanel
{
private:
    bool isOpen;
    CelestialBody* selectedPlanet;

    // Planet database with detailed information
    struct PlanetData {
        std::string fullName;
        std::string type;
        std::string description;
        std::string atmosphere;
        std::string composition;
        std::string temperature;
        std::string funFacts[3];
        ImVec4 color;
    };

    std::map<std::string, PlanetData> planetDatabase;

public:
    PlanetInformationPanel() : isOpen(false), selectedPlanet(nullptr)
    {
        initializePlanetDatabase();
    }

    void initializePlanetDatabase()
    {
        // Sun
        planetDatabase["Sun"] = {
            "The Sun (Sol)",
            "G-type Main-Sequence Star",
            "The Sun is the star at the center of our Solar System. It is a nearly perfect sphere of hot plasma, "
            "heated to incandescence by nuclear fusion reactions in its core. The Sun accounts for about 99.86% "
            "of the total mass of the Solar System.",
            "Hydrogen (73%), Helium (25%), Other (2%)",
            "Plasma - Primarily hydrogen and helium undergoing nuclear fusion",
            "Surface: ~5,500°C | Core: ~15,000,000°C",
            {
                "The Sun converts 600 million tons of hydrogen into helium every second",
                "Light from the Sun takes 8 minutes 20 seconds to reach Earth",
                "The Sun is 4.6 billion years old and halfway through its life cycle"
            },
            ImVec4(1.0f, 0.9f, 0.0f, 1.0f)
        };

        // Mercury
        planetDatabase["Mercury"] = {
            "Mercury - The Swift Planet",
            "Terrestrial Planet",
            "Mercury is the smallest and innermost planet in the Solar System. Its orbit around the Sun takes 87.97 "
            "Earth days, the shortest of all the planets. It is named after the Roman deity Mercury, the messenger of the gods.",
            "Thin exosphere: Oxygen (42%), Sodium (29%), Hydrogen (22%)",
            "Rocky with large iron core - Silicate crust and mantle",
            "Day: 430°C | Night: -180°C (Extreme temperature variation)",
            {
                "Mercury has the most eccentric orbit of all planets",
                "A day on Mercury lasts 176 Earth days (2 Mercury years!)",
                "Despite being closest to the Sun, Venus is actually hotter"
            },
            ImVec4(0.7f, 0.7f, 0.7f, 1.0f)
        };

        // Venus
        planetDatabase["Venus"] = {
            "Venus - Earth's Twin",
            "Terrestrial Planet",
            "Venus is the second planet from the Sun. It is named after the Roman goddess of love and beauty. "
            "As the brightest natural object in Earth's night sky after the Moon, Venus can cast shadows and can be "
            "visible to the naked eye in broad daylight.",
            "Dense: CO₂ (96.5%), Nitrogen (3.5%) - Extreme greenhouse effect",
            "Rocky surface with widespread volcanic features",
            "Surface: 465°C (Hottest planet in the Solar System)",
            {
                "Venus rotates backwards (retrograde) compared to other planets",
                "A day on Venus (243 Earth days) is longer than its year (225 Earth days)",
                "Surface pressure is 92 times that of Earth - equivalent to 900m underwater"
            },
            ImVec4(0.9f, 0.7f, 0.3f, 1.0f)
        };

        // Earth
        planetDatabase["Earth"] = {
            "Earth - The Blue Marble",
            "Terrestrial Planet",
            "Earth is the third planet from the Sun and the only astronomical object known to harbor life. "
            "About 29% of Earth's surface is land consisting of continents and islands. The remaining 71% is "
            "covered with water, mostly by oceans but also lakes, rivers, and other fresh water.",
            "Nitrogen (78%), Oxygen (21%), Argon (0.9%), CO₂ (0.04%)",
            "Rocky with iron-nickel core, silicate mantle, and oceanic/continental crust",
            "Average: 15°C (Habitable zone - supports liquid water)",
            {
                "Earth is the only known planet with plate tectonics",
                "The Earth's magnetic field protects us from harmful solar radiation",
                "Earth is the densest planet in the Solar System"
            },
            ImVec4(0.2f, 0.4f, 0.8f, 1.0f)
        };

        // Mars
        planetDatabase["Mars"] = {
            "Mars - The Red Planet",
            "Terrestrial Planet",
            "Mars is the fourth planet from the Sun and the second-smallest planet in the Solar System, being larger "
            "than only Mercury. Named after the Roman god of war, it is often referred to as the 'Red Planet' because "
            "the iron oxide prevalent on its surface gives it a reddish appearance.",
            "Thin: CO₂ (95%), Nitrogen (2.8%), Argon (2%)",
            "Rocky with iron oxide (rust) covering surface, frozen water at poles",
            "Average: -63°C | Poles: -125°C | Equator: 20°C",
            {
                "Mars has the largest volcano in the Solar System - Olympus Mons (21km high)",
                "A day on Mars is 24 hours 37 minutes - very similar to Earth",
                "Mars has two small moons: Phobos and Deimos"
            },
            ImVec4(0.8f, 0.3f, 0.2f, 1.0f)
        };

        // Jupiter
        planetDatabase["Jupiter"] = {
            "Jupiter - King of the Planets",
            "Gas Giant",
            "Jupiter is the fifth planet from the Sun and the largest in the Solar System. It is a gas giant with "
            "a mass one-thousandth that of the Sun, but two-and-a-half times that of all the other planets in the "
            "Solar System combined. Jupiter's iconic Great Red Spot is a giant storm larger than Earth.",
            "Hydrogen (90%), Helium (10%), traces of methane, ammonia",
            "No solid surface - dense atmosphere transitioning to liquid metallic hydrogen",
            "Cloud tops: -145°C | Core: ~24,000°C",
            {
                "Jupiter has 95 known moons, including the 4 large Galilean moons",
                "Jupiter's magnetic field is 14 times stronger than Earth's",
                "The Great Red Spot has been raging for at least 400 years"
            },
            ImVec4(0.8f, 0.6f, 0.4f, 1.0f)
        };

        // Saturn
        planetDatabase["Saturn"] = {
            "Saturn - Lord of the Rings",
            "Gas Giant",
            "Saturn is the sixth planet from the Sun and the second-largest in the Solar System, after Jupiter. "
            "It is a gas giant with an average radius about nine times that of Earth. Saturn is best known for its "
            "beautiful and extensive ring system, which is made mostly of ice particles with a smaller amount of debris.",
            "Hydrogen (96%), Helium (3%), traces of ammonia and methane",
            "No solid surface - similar to Jupiter but less dense (would float in water!)",
            "Cloud tops: -178°C | Core: ~11,700°C",
            {
                "Saturn's rings are only about 10 meters thick despite being 280,000 km wide",
                "Saturn has 146 known moons - Titan is larger than Mercury",
                "Saturn is the least dense planet - it would float in a giant bathtub"
            },
            ImVec4(0.9f, 0.8f, 0.6f, 1.0f)
        };

        // Uranus
        planetDatabase["Uranus"] = {
            "Uranus - The Tilted Giant",
            "Ice Giant",
            "Uranus is the seventh planet from the Sun. It has the third-largest planetary radius and fourth-largest "
            "planetary mass in the Solar System. Uranus is unique among the planets in that it rotates on its side, "
            "with an axial tilt of 98 degrees. This gives it extreme seasonal variations.",
            "Hydrogen (83%), Helium (15%), Methane (2%) - gives blue-green color",
            "Ice giant - water, methane, and ammonia ices over rocky core",
            "Cloud tops: -224°C (Coldest planetary atmosphere in Solar System)",
            {
                "Uranus rotates on its side - likely hit by Earth-sized object billions of years ago",
                "Uranus has 27 known moons, all named after Shakespeare and Alexander Pope characters",
                "A year on Uranus lasts 84 Earth years - each pole gets 42 years of sunlight/darkness"
            },
            ImVec4(0.4f, 0.7f, 0.8f, 1.0f)
        };

        // Neptune
        planetDatabase["Neptune"] = {
            "Neptune - The Windy Planet",
            "Ice Giant",
            "Neptune is the eighth and farthest known planet from the Sun. It is the fourth-largest planet by diameter "
            "and the third-most-massive planet. Neptune has the strongest winds of any planet in the Solar System, "
            "with speeds reaching up to 2,100 km/h.",
            "Hydrogen (80%), Helium (19%), Methane (1.5%) - gives deep blue color",
            "Ice giant - similar to Uranus with water-ammonia ocean over rocky core",
            "Cloud tops: -214°C | Warmer than Uranus despite being farther from Sun",
            {
                "Neptune has the fastest winds in the Solar System - up to 2,100 km/h",
                "Neptune was the first planet found by mathematical prediction rather than observation",
                "A year on Neptune lasts 165 Earth years - it's only completed one orbit since discovery in 1846"
            },
            ImVec4(0.2f, 0.3f, 0.9f, 1.0f)
        };

        // Black Hole
        planetDatabase["Black Hole"] = {
            "Schwarzschild Black Hole",
            "Stellar Black Hole",
            "A black hole is a region of spacetime where gravity is so strong that nothing—no particles or even "
            "electromagnetic radiation such as light—can escape from it. The event horizon is the boundary beyond "
            "which nothing can return. Black holes are among the most extreme objects in the universe.",
            "N/A - No atmosphere (vacuum)",
            "Singularity - Infinite density point at center, surrounded by event horizon",
            "Near event horizon: Extreme time dilation and gravitational effects",
            {
                "Time slows down near a black hole - spend an hour near it, years pass elsewhere",
                "If you fell in, you'd be 'spaghettified' - stretched into thin strings by tidal forces",
                "Black holes can spin at nearly the speed of light, dragging spacetime with them"
            },
            ImVec4(0.1f, 0.0f, 0.2f, 1.0f)
        };
    }

    void open(CelestialBody* planet)
    {
        selectedPlanet = planet;
        isOpen = true;
    }

    void close()
    {
        isOpen = false;
        selectedPlanet = nullptr;
    }

    bool isActive() const { return isOpen; }

    void render(const ScienceSystem& science)
    {
        if (!isOpen || !selectedPlanet) return;

        // Full-screen window
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        if (viewport)
        {
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
        }
        else
        {
            ImGuiIO& io = ImGui::GetIO();
            ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
            ImGui::SetNextWindowSize(io.DisplaySize);
        }

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

        if (ImGui::Begin("Planet Information Panel", &isOpen, flags))
        {
            std::string planetName = selectedPlanet->getName();

            // Get planet data
            PlanetData data;
            if (planetDatabase.find(planetName) != planetDatabase.end()) {
                data = planetDatabase[planetName];
            } else {
                data.fullName = planetName;
                data.type = "Unknown";
                data.description = "No data available for this celestial body.";
            }

            // Header section
            ImGui::PushStyleColor(ImGuiCol_Text, data.color);
            ImGui::SetWindowFontScale(2.0f);
            ImGui::Text("%s", data.fullName.c_str());
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor();

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
            ImGui::SetWindowFontScale(1.2f);
            ImGui::Text("%s", data.type.c_str());
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor();

            ImGui::Separator();
            ImGui::Spacing();

            // Two-column layout
            ImGui::Columns(2, "planet_columns", true);

            // LEFT COLUMN - Physical Data
            ImGui::SetWindowFontScale(1.3f);
            ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Physical Properties");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Spacing();

            // Display physical properties from the physics body
            Body& phys = selectedPlanet->getPhysicsBody();

            ImGui::Text("Radius: %.1f units", selectedPlanet->getRadius());
            ImGui::Text("Mass: %.2e kg", phys.mass);

            // Calculate surface gravity
            const float G = 6.674e-11f;
            float surfaceGravity = (G * phys.mass) / (selectedPlanet->getRadius() * selectedPlanet->getRadius());
            ImGui::Text("Surface Gravity: %.2f m/s²", surfaceGravity);

            ImGui::Text("Position: (%.1f, %.1f, %.1f)", phys.position.x, phys.position.y, phys.position.z);

            if (phys.velocity.length() > 0.01f) {
                ImGui::Text("Orbital Velocity: %.2f m/s", phys.velocity.length());
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Atmospheric Data
            if (!data.atmosphere.empty() && data.atmosphere != "N/A - No atmosphere (vacuum)") {
                ImGui::SetWindowFontScale(1.3f);
                ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.8f, 1.0f), "Atmosphere");
                ImGui::SetWindowFontScale(1.0f);
                ImGui::Spacing();
                ImGui::TextWrapped("%s", data.atmosphere.c_str());
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Spacing();
            }

            // Composition
            ImGui::SetWindowFontScale(1.3f);
            ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.4f, 1.0f), "Composition");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Spacing();
            ImGui::TextWrapped("%s", data.composition.c_str());
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Temperature
            ImGui::SetWindowFontScale(1.3f);
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Temperature");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Spacing();
            ImGui::TextWrapped("%s", data.temperature.c_str());

            // RIGHT COLUMN - Description and Science
            ImGui::NextColumn();

            ImGui::SetWindowFontScale(1.3f);
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "Overview");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Spacing();
            ImGui::TextWrapped("%s", data.description.c_str());
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Fun Facts
            ImGui::SetWindowFontScale(1.3f);
            ImGui::TextColored(ImVec4(0.8f, 0.4f, 1.0f, 1.0f), "Fascinating Facts");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Spacing();

            for (int i = 0; i < 3; ++i) {
                if (!data.funFacts[i].empty()) {
                    ImGui::Bullet();
                    ImGui::TextWrapped("%s", data.funFacts[i].c_str());
                    ImGui::Spacing();
                }
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Science Data Section
            ImGui::SetWindowFontScale(1.3f);
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Science Data");
            ImGui::SetWindowFontScale(1.0f);
            ImGui::Spacing();

            // Check if this planet has been scanned
            auto experiments = science.getExperimentsForPlanet(planetName);
            bool hasScannedData = false;
            int completedExperiments = 0;

            for (const auto& exp : experiments) {
                if (exp.collected) {
                    hasScannedData = true;
                    completedExperiments++;
                }
            }

            if (hasScannedData) {
                ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Status: SCANNED");
                ImGui::Text("Experiments Completed: %d / %d", completedExperiments, static_cast<int>(experiments.size()));
                ImGui::Spacing();

                ImGui::Text("Available Experiments:");
                for (const auto& exp : experiments) {
                    std::string expName = getExperimentTypeName(exp.type);
                    if (exp.collected) {
                        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "  [COMPLETE] %s (+%.0f science)",
                                         expName.c_str(), exp.scienceValue);
                    } else {
                        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "  [PENDING] %s (%.0f science)",
                                         expName.c_str(), exp.scienceValue);
                    }
                }
            } else {
                ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "Status: NOT YET SCANNED");
                ImGui::Text("Get within 50 units and press F7 to scan");
                ImGui::Spacing();
                ImGui::Text("Potential Science: %d points", science.getPotentialScience(planetName));
                ImGui::Text("Available Experiments: %d", static_cast<int>(experiments.size()));
            }

            ImGui::Columns(1);
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // Close button
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2.0f - 60.0f);
            if (ImGui::Button("Close (ESC)", ImVec2(120, 40)) || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                close();
            }
        }
        ImGui::End();

        ImGui::PopStyleVar(2);
    }
};
