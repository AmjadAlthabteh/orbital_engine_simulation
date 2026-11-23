#include "SaveSystem.hpp"

#include <cassert>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <limits>

int main()
{
    const auto testDirectory = std::filesystem::temp_directory_path() /
        "orbital-save-system-tests";
    std::error_code error;
    std::filesystem::remove_all(testDirectory, error);

    SaveSystem saves(testDirectory.string());
    SaveData expected;
    expected.saveName = "Pilot \"Nova\"\nCampaign";
    expected.currentTarget = "Lagrange \\ Point";
    expected.shipPosition = Vec3(12.5f, -4.0f, 88.0f);
    expected.fuel = 321.0f;
    expected.oxygen = 73.0f;
    expected.health = 64.0f;

    assert(saves.save(expected, "mission.sav"));
    SaveData actual;
    assert(saves.load(actual, "mission.sav"));
    assert(actual.saveName == expected.saveName);
    assert(!actual.saveDate.empty() && actual.saveDate != "true");
    assert(actual.currentTarget == expected.currentTarget);
    assert(actual.shipPosition.x == expected.shipPosition.x);
    assert(actual.fuel == expected.fuel);
    assert(actual.oxygen == expected.oxygen);
    assert(actual.health == expected.health);

    assert(!saves.save(expected, "../outside.sav"));
    SaveData invalid = expected;
    invalid.fuel = std::numeric_limits<float>::quiet_NaN();
    assert(!saves.save(invalid, "invalid.sav"));

    expected.saveName = "Second";
    assert(saves.save(expected, "alpha.sav"));
    const auto discovered = saves.listSaves();
    assert((discovered == std::vector<std::string>{"alpha.sav", "mission.sav"}));

    SaveData preserved;
    preserved.fuel = 42.0f;
    {
        std::ofstream malformed(testDirectory / "broken.sav");
        malformed << "{\n\"fuel\": not-a-number,\n}\n";
    }
    assert(!saves.load(preserved, "broken.sav"));
    assert(preserved.fuel == 42.0f);

    std::filesystem::remove_all(testDirectory, error);
    return 0;
}
