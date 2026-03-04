#include "VectorRenderer.hpp"
#include <cmath>

VectorRenderer::VectorRenderer()
{
    setupBuffers();
}

VectorRenderer::~VectorRenderer()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void VectorRenderer::setupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Position (3 floats) + Color (3 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void VectorRenderer::addVelocityVector(const Vec3& origin, const Vec3& velocity)
{
    VectorArrow arrow;
    arrow.origin = origin;
    arrow.direction = velocity.normalize();
    arrow.magnitude = velocity.length() * 2.0f; // Scale for visibility
    arrow.color = Vec3(0.0f, 1.0f, 0.5f); // Green-cyan for velocity
    arrow.active = true;

    velocityVectors.push_back(arrow);
}

void VectorRenderer::addForceVector(const Vec3& origin, const Vec3& force)
{
    float forceMag = force.length();
    if (forceMag < 0.01f)
        return; // Don't render tiny forces

    VectorArrow arrow;
    arrow.origin = origin;
    arrow.direction = force.normalize();
    arrow.magnitude = std::log(forceMag + 1.0f) * 3.0f; // Logarithmic scale for visibility
    arrow.color = Vec3(1.0f, 0.3f, 0.0f); // Orange-red for forces
    arrow.active = true;

    forceVectors.push_back(arrow);
}

void VectorRenderer::createArrowGeometry(const Vec3& origin, const Vec3& direction, float magnitude, const Vec3& color)
{
    Vec3 endpoint = origin + direction * magnitude;

    // Arrow shaft (line from origin to endpoint)
    vertexData.push_back(origin.x);
    vertexData.push_back(origin.y);
    vertexData.push_back(origin.z);
    vertexData.push_back(color.x);
    vertexData.push_back(color.y);
    vertexData.push_back(color.z);

    vertexData.push_back(endpoint.x);
    vertexData.push_back(endpoint.y);
    vertexData.push_back(endpoint.z);
    vertexData.push_back(color.x);
    vertexData.push_back(color.y);
    vertexData.push_back(color.z);

    // Arrow head - create perpendicular vectors
    Vec3 arbitrary = (std::abs(direction.y) < 0.9f) ? Vec3(0, 1, 0) : Vec3(1, 0, 0);
    Vec3 perpendicular1 = direction.cross(arbitrary).normalize();
    Vec3 perpendicular2 = direction.cross(perpendicular1).normalize();

    float headSize = magnitude * 0.2f;
    Vec3 headBase = origin + direction * (magnitude * 0.8f);

    // Arrow head triangles (4 lines making a cone)
    for (int i = 0; i < 4; ++i)
    {
        float angle = (i * 3.14159f * 2.0f) / 4.0f;
        Vec3 headPoint = headBase +
            (perpendicular1 * std::cos(angle) + perpendicular2 * std::sin(angle)) * headSize;

        vertexData.push_back(endpoint.x);
        vertexData.push_back(endpoint.y);
        vertexData.push_back(endpoint.z);
        vertexData.push_back(color.x * 1.2f);
        vertexData.push_back(color.y * 1.2f);
        vertexData.push_back(color.z * 1.2f);

        vertexData.push_back(headPoint.x);
        vertexData.push_back(headPoint.y);
        vertexData.push_back(headPoint.z);
        vertexData.push_back(color.x * 1.2f);
        vertexData.push_back(color.y * 1.2f);
        vertexData.push_back(color.z * 1.2f);
    }
}

void VectorRenderer::updateBuffers()
{
    vertexData.clear();

    // Create geometry for all velocity vectors
    for (const auto& arrow : velocityVectors)
    {
        if (arrow.active && arrow.magnitude > 0.1f)
        {
            createArrowGeometry(arrow.origin, arrow.direction, arrow.magnitude, arrow.color);
        }
    }

    // Create geometry for all force vectors
    for (const auto& arrow : forceVectors)
    {
        if (arrow.active && arrow.magnitude > 0.1f)
        {
            createArrowGeometry(arrow.origin, arrow.direction, arrow.magnitude, arrow.color);
        }
    }

    if (vertexData.empty())
        return;

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertexData.size() * sizeof(float),
        vertexData.data(),
        GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VectorRenderer::render()
{
    updateBuffers();

    if (vertexData.empty())
        return;

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertexData.size() / 6));
    glBindVertexArray(0);
}

void VectorRenderer::clear()
{
    velocityVectors.clear();
    forceVectors.clear();
}
