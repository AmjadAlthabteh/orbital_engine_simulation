#include "StarField.hpp"
#include <cstdlib>

StarField::StarField(int count)
{
    for (int i = 0; i < count; ++i)
    {
        float x = ((rand() % 2000) - 1000) / 10.0f;
        float y = ((rand() % 2000) - 1000) / 10.0f;
        float z = -((rand() % 2000) / 10.0f);

        stars.emplace_back(x, y, z);
    }
}

void StarField::update(float deltaTime)
{
    // Optional: animate stars later
}

const std::vector<Vec3>& StarField::getStars() const
{
    return stars;
}