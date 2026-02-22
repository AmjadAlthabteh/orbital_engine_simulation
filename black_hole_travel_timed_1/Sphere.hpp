#pragma once
#include <vector>

class Sphere
{
private:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;

public:
    Sphere(float radius = 1.0f, int sectorCount = 36, int stackCount = 18);

    void draw() const;
};