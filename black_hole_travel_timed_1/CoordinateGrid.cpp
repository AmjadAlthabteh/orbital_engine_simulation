#include "CoordinateGrid.hpp"
#include <cmath>

CoordinateGrid::CoordinateGrid(float gridSize, float spacing)
    : gridSize(gridSize), spacing(spacing), isVisible(true), animationTime(0.0f)
{
    gridVAO = gridVBO = 0;
    axesVAO = axesVBO = 0;
    pointVAO = pointVBO = 0;

    generateGrid();
    generateAxes();
    setupBuffers();
}

CoordinateGrid::~CoordinateGrid()
{
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);
    glDeleteVertexArrays(1, &axesVAO);
    glDeleteBuffers(1, &axesVBO);
    glDeleteVertexArrays(1, &pointVAO);
    glDeleteBuffers(1, &pointVBO);
}

void CoordinateGrid::generateGrid()
{
    gridVertexData.clear();
    pointVertexData.clear();

    Vec3 gridColor(0.15f, 0.2f, 0.3f);  // Subtle blue-gray

    // Generate grid lines parallel to XZ plane (Y = 0)
    for (float x = -gridSize; x <= gridSize; x += spacing)
    {
        // Line parallel to Z axis
        gridVertexData.push_back(x);
        gridVertexData.push_back(0.0f);
        gridVertexData.push_back(-gridSize);
        gridVertexData.push_back(gridColor.x);
        gridVertexData.push_back(gridColor.y);
        gridVertexData.push_back(gridColor.z);

        gridVertexData.push_back(x);
        gridVertexData.push_back(0.0f);
        gridVertexData.push_back(gridSize);
        gridVertexData.push_back(gridColor.x);
        gridVertexData.push_back(gridColor.y);
        gridVertexData.push_back(gridColor.z);
    }

    for (float z = -gridSize; z <= gridSize; z += spacing)
    {
        // Line parallel to X axis
        gridVertexData.push_back(-gridSize);
        gridVertexData.push_back(0.0f);
        gridVertexData.push_back(z);
        gridVertexData.push_back(gridColor.x);
        gridVertexData.push_back(gridColor.y);
        gridVertexData.push_back(gridColor.z);

        gridVertexData.push_back(gridSize);
        gridVertexData.push_back(0.0f);
        gridVertexData.push_back(z);
        gridVertexData.push_back(gridColor.x);
        gridVertexData.push_back(gridColor.y);
        gridVertexData.push_back(gridColor.z);
    }

    // Generate grid intersection points (mathematical coordinates)
    for (float x = -gridSize; x <= gridSize; x += spacing)
    {
        for (float z = -gridSize; z <= gridSize; z += spacing)
        {
            // Point at grid intersection
            pointVertexData.push_back(x);
            pointVertexData.push_back(0.0f);
            pointVertexData.push_back(z);

            // Color: brighter at major intersections
            bool isMajor = (std::fmod(std::abs(x), spacing * 5.0f) < 0.1f &&
                            std::fmod(std::abs(z), spacing * 5.0f) < 0.1f);
            if (isMajor)
            {
                pointVertexData.push_back(0.4f);
                pointVertexData.push_back(0.6f);
                pointVertexData.push_back(0.8f);
            }
            else
            {
                pointVertexData.push_back(0.2f);
                pointVertexData.push_back(0.3f);
                pointVertexData.push_back(0.4f);
            }

            // Size
            pointVertexData.push_back(isMajor ? 4.0f : 2.0f);
        }
    }
}

void CoordinateGrid::generateAxes()
{
    axesVertexData.clear();

    float axisLength = gridSize * 1.2f;

    // X axis (RED)
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(1.0f);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(0.0f);

    axesVertexData.push_back(axisLength);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(1.0f);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(0.0f);

    // Y axis (GREEN)
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(1.0f);
    axesVertexData.push_back(0.0f);

    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(axisLength);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(1.0f);
    axesVertexData.push_back(0.0f);

    // Z axis (BLUE)
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(1.0f);

    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(axisLength);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(0.0f);
    axesVertexData.push_back(1.0f);
}

void CoordinateGrid::setupBuffers()
{
    // Grid lines
    glGenVertexArrays(1, &gridVAO);
    glGenBuffers(1, &gridVBO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertexData.size() * sizeof(float),
                 gridVertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Axes
    glGenVertexArrays(1, &axesVAO);
    glGenBuffers(1, &axesVBO);
    glBindVertexArray(axesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axesVBO);
    glBufferData(GL_ARRAY_BUFFER, axesVertexData.size() * sizeof(float),
                 axesVertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Points
    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &pointVBO);
    glBindVertexArray(pointVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
    glBufferData(GL_ARRAY_BUFFER, pointVertexData.size() * sizeof(float),
                 pointVertexData.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void CoordinateGrid::updateBuffers()
{
    // Update point buffer if needed
}

void CoordinateGrid::update(float deltaTime)
{
    animationTime += deltaTime;

    // Subtle pulsing effect on grid points
    // (Could be used for custom points in the future)
}

void CoordinateGrid::render()
{
    if (!isVisible)
        return;

    // Render grid lines
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(gridVertexData.size() / 6));
    glBindVertexArray(0);

    // Render axes (thicker, brighter)
    glLineWidth(3.0f);
    glBindVertexArray(axesVAO);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);
    glLineWidth(2.0f);

    // Render grid points
    glBindVertexArray(pointVAO);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(pointVertexData.size() / 7));
    glBindVertexArray(0);
}

void CoordinateGrid::addPoint(const Vec3& position, const Vec3& color, const std::string& label)
{
    GridPoint point;
    point.position = position;
    point.color = color;
    point.size = 8.0f;
    point.showLabel = !label.empty();
    point.label = label;
    customPoints.push_back(point);
}

void CoordinateGrid::clearCustomPoints()
{
    customPoints.clear();
}
