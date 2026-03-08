#pragma once
#include <glad/glad.h>
#include <vector>
#include <string>
#include "Vec3.hpp"
#include "Shader.hpp"
#include "Mat4.hpp"

// Represents a 3D coordinate point with label
struct GridPoint
{
    Vec3 position;
    Vec3 color;
    float size;
    bool showLabel;
    std::string label;
};

// 3D coordinate grid system with labeled points
// Shows mathematical (x, y, z) coordinates in 3D space
class CoordinateGrid
{
public:
    CoordinateGrid(float gridSize = 100.0f, float spacing = 10.0f);
    ~CoordinateGrid();

    void update(float deltaTime);
    void render();

    // Add custom labeled points
    void addPoint(const Vec3& position, const Vec3& color, const std::string& label = "");
    void clearCustomPoints();

    void setVisible(bool visible) { isVisible = visible; }
    bool getVisible() const { return isVisible; }

private:
    void generateGrid();
    void generateAxes();
    void setupBuffers();
    void updateBuffers();

    std::vector<float> gridVertexData;   // Grid lines
    std::vector<float> axesVertexData;   // X, Y, Z axes
    std::vector<float> pointVertexData;  // Grid intersection points
    std::vector<GridPoint> customPoints;

    GLuint gridVAO, gridVBO;
    GLuint axesVAO, axesVBO;
    GLuint pointVAO, pointVBO;

    float gridSize;
    float spacing;
    bool isVisible;
    float animationTime;
};
