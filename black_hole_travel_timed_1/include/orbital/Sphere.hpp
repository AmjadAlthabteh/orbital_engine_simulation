#pragma once
#include <glad/glad.h>
#include <vector>

// OPTIMIZATION: LOD (Level of Detail) enum
enum class SphereLOD
{
    HIGH,       // 36 sectors x 18 stacks (high detail, near objects)
    MEDIUM,     // 24 sectors x 12 stacks (medium detail)
    LOW         // 12 sectors x 6 stacks (low detail, far objects)
};

class Sphere
{
private:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
    int sectorCount;
    int stackCount;

public:
    Sphere(float radius = 1.0f, int sectorCount = 36, int stackCount = 18);

    void draw() const;

    // OPTIMIZATION: Get detail level counts for LOD system
    static void getLODCounts(SphereLOD lod, int& outSectors, int& outStacks);

    // Get current detail counts
    int getSectorCount() const { return sectorCount; }
    int getStackCount() const { return stackCount; }
};