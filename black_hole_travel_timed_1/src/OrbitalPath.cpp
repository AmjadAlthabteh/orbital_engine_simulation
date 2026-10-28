#include "OrbitalPath.hpp"
#include <cmath>

namespace
{
    constexpr float kMinMu = 0.000001f;
    constexpr float kMinAngularMomentum = 0.000001f;
    constexpr float kMaxRenderableEccentricity = 0.95f;
}

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
    if (numPoints <= 0)
        return;

    pathPoints.reserve(static_cast<size_t>(numPoints) + 1);

    Vec3 r = body.position - centralBody.position;
    Vec3 v = body.velocity - centralBody.velocity;

    float mu = G * centralBody.mass;
    if (mu <= kMinMu)
        return;

    Vec3 h = r.cross(v);
    float h_mag = h.length();
    if (h_mag <= kMinAngularMomentum)
        return;

    Vec3 radialDirection = r.normalizedOrZero();
    if (radialDirection.lengthSquared() == 0.0f)
        return;

    Vec3 e_vec = (v.cross(h) * (1.0f / mu)) - radialDirection;
    float e = e_vec.length();

    const float eccentricityTerm = 1.0f - e * e;
    if (eccentricityTerm <= 0.0001f || e > kMaxRenderableEccentricity)
        return;

    const float semiLatusRectum = h_mag * h_mag / mu;
    float a = semiLatusRectum / eccentricityTerm;

    if (a <= 0.0f || !std::isfinite(a))
    {
        return;
    }

    // The eccentricity vector points toward periapsis. The central body belongs
    // at a focus, not at the geometric center of the rendered ellipse.
    Vec3 periapsis = e_vec.normalizedOrZero();
    if (periapsis.lengthSquared() == 0.0f)
        periapsis = radialDirection;

    Vec3 normal = h.normalizedOrZero();
    Vec3 minor_axis = normal.cross(periapsis);
    if (minor_axis.lengthSquared() == 0.0f)
        return;

    constexpr float twoPi = 6.28318530718f;
    const float thetaStep = twoPi / static_cast<float>(numPoints);
    const float cosStep = std::cos(thetaStep);
    const float sinStep = std::sin(thetaStep);
    float cosTheta = 1.0f;
    float sinTheta = 0.0f;

    for (int i = 0; i <= numPoints; ++i)
    {
        const float radius = semiLatusRectum / (1.0f + e * cosTheta);
        Vec3 point = centralBody.position +
                     periapsis * (radius * cosTheta) +
                     minor_axis * (radius * sinTheta);
        pathPoints.push_back(point);

        const float nextCos = cosTheta * cosStep - sinTheta * sinStep;
        sinTheta = sinTheta * cosStep + cosTheta * sinStep;
        cosTheta = nextCos;
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
