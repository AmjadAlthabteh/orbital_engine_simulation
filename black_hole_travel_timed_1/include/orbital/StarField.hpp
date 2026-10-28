#pragma once
#include "Entity.hpp"
#include "Vec3.hpp"
#include <vector>
#include <glad/glad.h>

struct Star
{
    Vec3 position;
    float brightness;
    float size;
    Vec3 color;
};

class StarField : public Entity
{
private:
    std::vector<Star> stars;
    unsigned int VAO, VBO;
    std::vector<float> vertexData;

public:
    StarField(int count = 5000);
    ~StarField();

    void update(float deltaTime) override;
    void render();

    const std::vector<Star>& getStars() const;

private:
    void setupBuffers();
    void updateBuffers();
    void generateStars(int count);
};