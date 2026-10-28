#include "PredictionMarker.hpp"
#include "Shader.hpp"
#include "Mat4.hpp"
#include <algorithm>

PredictionMarker::PredictionMarker()
    : markerSphere(0.3f)
{
    setupBuffers();
}

PredictionMarker::~PredictionMarker()
{
    glDeleteVertexArrays(1, &lineVAO);
    glDeleteBuffers(1, &lineVBO);
}

void PredictionMarker::setupBuffers()
{
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void PredictionMarker::addCollisionMarker(const Vec3& position, float timeToCollision)
{
    Marker marker;
    marker.position = position;
    marker.size = 0.5f;
    marker.type = MarkerType::COLLISION_POINT;
    marker.lifetime = 5.0f;
    marker.active = true;

    // Color based on urgency - red for imminent, yellow for distant
    if (timeToCollision < 10.0f)
        marker.color = Vec3(1.0f, 0.0f, 0.0f); // Red - imminent
    else if (timeToCollision < 30.0f)
        marker.color = Vec3(1.0f, 0.5f, 0.0f); // Orange
    else
        marker.color = Vec3(1.0f, 1.0f, 0.0f); // Yellow

    markers.push_back(marker);
}

void PredictionMarker::addTrajectoryPoint(const Vec3& position, const Vec3& color)
{
    Marker marker;
    marker.position = position;
    marker.color = color;
    marker.size = 0.2f;
    marker.type = MarkerType::TRAJECTORY_POINT;
    marker.lifetime = 3.0f;
    marker.active = true;

    markers.push_back(marker);
}

void PredictionMarker::addLagrangePoint(const Vec3& position)
{
    Marker marker;
    marker.position = position;
    marker.color = Vec3(0.0f, 1.0f, 1.0f); // Cyan
    marker.size = 0.4f;
    marker.type = MarkerType::LAGRANGE_POINT;
    marker.lifetime = 10.0f;
    marker.active = true;

    markers.push_back(marker);
}

void PredictionMarker::addPeriapsis(const Vec3& position)
{
    Marker marker;
    marker.position = position;
    marker.color = Vec3(0.0f, 1.0f, 0.0f); // Green - closest point
    marker.size = 0.35f;
    marker.type = MarkerType::PERIAPSIS;
    marker.lifetime = 8.0f;
    marker.active = true;

    markers.push_back(marker);
}

void PredictionMarker::addApoapsis(const Vec3& position)
{
    Marker marker;
    marker.position = position;
    marker.color = Vec3(0.5f, 0.0f, 1.0f); // Purple - farthest point
    marker.size = 0.35f;
    marker.type = MarkerType::APOAPSIS;
    marker.lifetime = 8.0f;
    marker.active = true;

    markers.push_back(marker);
}

void PredictionMarker::update(float deltaTime)
{
    // Update marker lifetimes and remove expired ones
    for (auto& marker : markers)
    {
        if (marker.active)
        {
            marker.lifetime -= deltaTime;
            if (marker.lifetime <= 0.0f)
            {
                marker.active = false;
            }
        }
    }

    // Remove inactive markers
    markers.erase(
        std::remove_if(markers.begin(), markers.end(),
            [](const Marker& m) { return !m.active; }),
        markers.end()
    );
}

void PredictionMarker::render(Shader* shader, const Mat4& view, const Mat4& projection)
{
    if (markers.empty())
        return;

    shader->use();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setBool("isSun", false);

    for (const auto& marker : markers)
    {
        if (!marker.active)
            continue;

        // Create model matrix for marker
        Mat4 translation = Mat4::translation(marker.position.x, marker.position.y, marker.position.z);
        Mat4 scale = Mat4::scale(marker.size);
        Mat4 model = translation * scale;

        shader->setMat4("model", model);
        shader->setVec3("objectColor", marker.color);
        shader->setVec3("viewPos", Vec3(0, 0, 0));
        shader->setVec3("lightPos", Vec3(0, 0, 0));

        markerSphere.draw();
    }
}

void PredictionMarker::clear()
{
    markers.clear();
}
