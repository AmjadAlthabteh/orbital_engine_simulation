#include "Trail.hpp"
#include <algorithm>
#include <cstddef>

Trail::Trail(size_t maxPoints_, const Vec3& color_)
    : maxPoints(maxPoints_), color(color_), needsUpdate(false)
{
    positions.reserve(maxPoints);
    setupBuffers();
}

Trail::~Trail()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Trail::setupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        maxPoints * 3 * sizeof(float),
        nullptr,
        GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Trail::addPoint(const Vec3& position)
{
    if (maxPoints == 0)
        return;

    positions.push_back(position);

    if (positions.size() > maxPoints)
    {
        const size_t overflow = positions.size() - maxPoints;
        positions.erase(positions.begin(), positions.begin() + static_cast<std::ptrdiff_t>(overflow));
    }

    needsUpdate = true;
}

void Trail::clear()
{
    positions.clear();
    needsUpdate = true;
}

void Trail::updateBuffers()
{
    // CRITICAL SAFETY: Check for empty vector before accessing
    if (!needsUpdate || positions.empty())
        return;

    // EXTRA SAFETY: Verify we have valid data
    if (positions.size() > 10000)  // Sanity check - trail should never be this long
    {
        positions.clear();
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER,
        0,
        positions.size() * 3 * sizeof(float),
        positions.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    needsUpdate = false;
}

void Trail::render()
{
    if (positions.size() < 2)
        return;

    updateBuffers();

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(positions.size()));
    glBindVertexArray(0);
}

void Trail::setColor(const Vec3& newColor)
{
    color = newColor;
}
