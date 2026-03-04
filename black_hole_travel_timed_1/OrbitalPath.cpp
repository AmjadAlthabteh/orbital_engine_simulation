#include "OrbitalPath.hpp"
#include <cmath>

OrbitalPath::OrbitalPath()
    : color(Vec3(0.3f, 0.3f, 0.5f))
{
    setupBuffers();
}

OrbitalPath::~OrbitalPath()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void OrbitalPath::setupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void OrbitalPath::calculatePath(const Body& body, const Body& centralBody, float G, int numPoints)
{
    pathPoints.clear();

    Vec3 r = body.position - centralBody.position;
    Vec3 v = body.velocity - centralBody.velocity;

    float mu = G * centralBody.mass;

    Vec3 h = r.cross(v);
    float h_mag = h.length();

    Vec3 e_vec = (v.cross(h) * (1.0f / mu)) - r.normalize();
    float e = e_vec.length();

    float a = h_mag * h_mag / (mu * (1.0f - e * e + 0.0001f));

    if (a < 0 || e > 0.95f)
    {
        return;
    }

    float b = a * std::sqrt(1.0f - e * e);

    Vec3 periapsis = e_vec.normalize() * (-1.0f);
    Vec3 normal = h.normalize();
    Vec3 minor_axis = normal.cross(periapsis);

    for (int i = 0; i <= numPoints; ++i)
    {
        float theta = (i * 2.0f * 3.14159265f) / numPoints;
        float x = a * std::cos(theta);
        float y = b * std::sin(theta);

        Vec3 point = centralBody.position + periapsis * x + minor_axis * y;
        pathPoints.push_back(point);
    }

    updateBuffers();
}

void OrbitalPath::updateBuffers()
{
    if (pathPoints.empty())
        return;

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        pathPoints.size() * 3 * sizeof(float),
        &pathPoints[0],
        GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void OrbitalPath::render()
{
    if (pathPoints.empty())
        return;

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_LOOP, 0, static_cast<GLsizei>(pathPoints.size()));
    glBindVertexArray(0);
}

void OrbitalPath::setColor(const Vec3& newColor)
{
    color = newColor;
}
