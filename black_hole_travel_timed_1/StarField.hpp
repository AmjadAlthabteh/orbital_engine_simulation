#pragma once
#include "Entity.hpp"
#include <vector>

class StarField : public Entity
{
private:
    std::vector<Vec3> stars;

public:
    StarField(int count);

    void update(float deltaTime) override;

    const std::vector<Vec3>& getStars() const;
};