#include "StarField.hpp"
#include <cstdlib>
#include <cmath>
#include <random>

StarField::StarField(int count)
{
    generateStars(count);
    setupBuffers();
}

StarField::~StarField()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void StarField::generateStars(int count)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    std::uniform_real_distribution<float> angleDis(0.0f, 6.28318530718f);
    std::uniform_real_distribution<float> radiusDis(50.0f, 500.0f);

    stars.clear();
    stars.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        Star star;

        float theta = angleDis(gen);
        float phi = std::acos(2.0f * dis(gen) - 1.0f);
        float radius = radiusDis(gen);

        star.position.x = radius * std::sin(phi) * std::cos(theta);
        star.position.y = radius * std::sin(phi) * std::sin(theta);
        star.position.z = radius * std::cos(phi);

        star.brightness = 0.3f + dis(gen) * 0.7f;
        star.size = 1.0f + dis(gen) * 2.0f;

        float colorChoice = dis(gen);
        if (colorChoice < 0.7f)
        {
            star.color = Vec3(1.0f, 1.0f, 1.0f);
        }
        else if (colorChoice < 0.85f)
        {
            star.color = Vec3(0.8f, 0.9f, 1.0f);
        }
        else if (colorChoice < 0.95f)
        {
            star.color = Vec3(1.0f, 0.9f, 0.7f);
        }
        else
        {
            star.color = Vec3(1.0f, 0.7f, 0.7f);
        }

        stars.push_back(star);
    }

    for (int i = 0; i < count / 10; ++i)
    {
        Star brightStar;

        float theta = angleDis(gen);
        float phi = std::acos(2.0f * dis(gen) - 1.0f);
        float radius = radiusDis(gen) * 1.2f;

        brightStar.position.x = radius * std::sin(phi) * std::cos(theta);
        brightStar.position.y = radius * std::sin(phi) * std::sin(theta);
        brightStar.position.z = radius * std::cos(phi);

        brightStar.brightness = 0.8f + dis(gen) * 0.2f;
        brightStar.size = 2.5f + dis(gen) * 2.0f;
        brightStar.color = Vec3(0.9f, 0.95f, 1.0f);

        stars.push_back(brightStar);
    }

    updateBuffers();
}

void StarField::setupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void StarField::updateBuffers()
{
    vertexData.clear();
    vertexData.reserve(stars.size() * 7);

    for (const auto& star : stars)
    {
        vertexData.push_back(star.position.x);
        vertexData.push_back(star.position.y);
        vertexData.push_back(star.position.z);
        vertexData.push_back(star.color.x * star.brightness);
        vertexData.push_back(star.color.y * star.brightness);
        vertexData.push_back(star.color.z * star.brightness);
        vertexData.push_back(star.size);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertexData.size() * sizeof(float),
        vertexData.data(),
        GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void StarField::update(float deltaTime)
{
    for (auto& star : stars)
    {
        star.brightness += (std::sin(deltaTime * 10.0f + star.position.x * 0.01f) * 0.001f);
        star.brightness = std::max(0.3f, std::min(1.0f, star.brightness));
    }
}

void StarField::render()
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(stars.size()));
    glBindVertexArray(0);
}

const std::vector<Star>& StarField::getStars() const
{
    return stars;
}