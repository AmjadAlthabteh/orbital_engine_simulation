#include "LagrangePoints.hpp"
#include <cmath>

const float PI = 3.14159265359f;

LagrangePoints::LagrangePoints()
    : markerSphere(0.4f), VAO(0), VBO(0), isVisible(true), animationTime(0.0f)
{
    setupBuffers();
}

LagrangePoints::~LagrangePoints()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void LagrangePoints::calculatePoints(const Body& primary, const Body& secondary)
{
    points.clear();

    // L1 - Between the two bodies (unstable)
    LagrangePoint L1;
    L1.type = LagrangeType::L1;
    L1.position = calculateL1(primary, secondary);
    L1.color = Vec3(1.0f, 0.8f, 0.2f);  // Yellow - unstable
    L1.size = 0.4f;
    L1.stable = false;
    L1.label = "L1 (Unstable)";
    points.push_back(L1);

    // L2 - Beyond secondary body (unstable)
    LagrangePoint L2;
    L2.type = LagrangeType::L2;
    L2.position = calculateL2(primary, secondary);
    L2.color = Vec3(1.0f, 0.6f, 0.1f);  // Orange - unstable
    L2.size = 0.4f;
    L2.stable = false;
    L2.label = "L2 (Unstable)";
    points.push_back(L2);

    // L3 - Opposite side of primary (unstable)
    LagrangePoint L3;
    L3.type = LagrangeType::L3;
    L3.position = calculateL3(primary, secondary);
    L3.color = Vec3(1.0f, 0.4f, 0.0f);  // Red-orange - unstable
    L3.size = 0.4f;
    L3.stable = false;
    L3.label = "L3 (Unstable)";
    points.push_back(L3);

    // L4 - 60° ahead in orbit (STABLE!)
    LagrangePoint L4;
    L4.type = LagrangeType::L4;
    L4.position = calculateL4(primary, secondary);
    L4.color = Vec3(0.2f, 1.0f, 0.3f);  // Green - stable!
    L4.size = 0.5f;
    L4.stable = true;
    L4.label = "L4 (STABLE!)";
    points.push_back(L4);

    // L5 - 60° behind in orbit (STABLE!)
    LagrangePoint L5;
    L5.type = LagrangeType::L5;
    L5.position = calculateL5(primary, secondary);
    L5.color = Vec3(0.3f, 1.0f, 0.2f);  // Bright green - stable!
    L5.size = 0.5f;
    L5.stable = true;
    L5.label = "L5 (STABLE!)";
    points.push_back(L5);

    updateBuffers();
}

Vec3 LagrangePoints::calculateL1(const Body& primary, const Body& secondary)
{
    // L1 is between the two bodies
    // Simplified calculation: weighted average closer to smaller body
    Vec3 direction = (secondary.position - primary.position);
    float distance = direction.length();
    direction = direction / distance;

    // Mass ratio affects position
    float massRatio = secondary.mass / (primary.mass + secondary.mass);
    float L1_distance = distance * (1.0f - std::pow(massRatio / 3.0f, 1.0f/3.0f));

    return primary.position + direction * L1_distance;
}

Vec3 LagrangePoints::calculateL2(const Body& primary, const Body& secondary)
{
    // L2 is beyond the secondary body (opposite side from primary)
    Vec3 direction = (secondary.position - primary.position);
    float distance = direction.length();
    direction = direction / distance;

    float massRatio = secondary.mass / (primary.mass + secondary.mass);
    float L2_distance = distance * (1.0f + std::pow(massRatio / 3.0f, 1.0f/3.0f));

    return primary.position + direction * L2_distance;
}

Vec3 LagrangePoints::calculateL3(const Body& primary, const Body& secondary)
{
    // L3 is on opposite side of primary from secondary
    Vec3 direction = (secondary.position - primary.position);
    float distance = direction.length();
    direction = direction / distance;

    float massRatio = secondary.mass / (primary.mass + secondary.mass);
    float L3_distance = distance * (1.0f + 5.0f * massRatio / 12.0f);

    return primary.position - direction * L3_distance;
}

Vec3 LagrangePoints::calculateL4(const Body& primary, const Body& secondary)
{
    // L4 forms an equilateral triangle with primary and secondary
    // It's 60° ahead of secondary in its orbit
    Vec3 toSecondary = secondary.position - primary.position;
    float distance = toSecondary.length();

    // Rotate vector by +60° around Y axis
    float angle = 60.0f * PI / 180.0f;
    float cosAngle = std::cos(angle);
    float sinAngle = std::sin(angle);

    Vec3 L4_direction(
        toSecondary.x * cosAngle - toSecondary.z * sinAngle,
        toSecondary.y,
        toSecondary.x * sinAngle + toSecondary.z * cosAngle
    );

    L4_direction = L4_direction / L4_direction.length();
    return primary.position + L4_direction * distance;
}

Vec3 LagrangePoints::calculateL5(const Body& primary, const Body& secondary)
{
    // L5 forms an equilateral triangle with primary and secondary
    // It's 60° behind secondary in its orbit
    Vec3 toSecondary = secondary.position - primary.position;
    float distance = toSecondary.length();

    // Rotate vector by -60° around Y axis
    float angle = -60.0f * PI / 180.0f;
    float cosAngle = std::cos(angle);
    float sinAngle = std::sin(angle);

    Vec3 L5_direction(
        toSecondary.x * cosAngle - toSecondary.z * sinAngle,
        toSecondary.y,
        toSecondary.x * sinAngle + toSecondary.z * cosAngle
    );

    L5_direction = L5_direction / L5_direction.length();
    return primary.position + L5_direction * distance;
}

void LagrangePoints::setupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Vertex layout: position(3), color(3), size(1) = 7 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void LagrangePoints::updateBuffers()
{
    vertexData.clear();
    vertexData.reserve(points.size() * 7);

    for (const auto& point : points)
    {
        vertexData.push_back(point.position.x);
        vertexData.push_back(point.position.y);
        vertexData.push_back(point.position.z);
        vertexData.push_back(point.color.x);
        vertexData.push_back(point.color.y);
        vertexData.push_back(point.color.z);
        vertexData.push_back(point.size * 5.0f);  // Make them visible
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertexData.size() * sizeof(float),
        vertexData.data(),
        GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LagrangePoints::update(float deltaTime)
{
    animationTime += deltaTime;

    // Make stable points (L4, L5) pulse gently
    for (auto& point : points)
    {
        if (point.stable)
        {
            float pulse = 1.0f + 0.2f * std::sin(animationTime * 2.0f);
            point.size = 0.5f * pulse;
        }
    }

    updateBuffers();
}

void LagrangePoints::render()
{
    if (!isVisible || points.empty())
        return;

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(points.size()));
    glBindVertexArray(0);
}
