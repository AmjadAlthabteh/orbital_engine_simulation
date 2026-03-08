#include "EngineParticles.hpp"
#include <random>
#include <cmath>
#include <algorithm>

EngineParticles::EngineParticles(int maxCount) : maxParticles(maxCount)
{
    particles.reserve(maxCount);
    setupBuffers();
}

EngineParticles::~EngineParticles()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void EngineParticles::emit(const Vec3& position, const Vec3& direction,
                           const Vec3& shipVelocity, float intensity, int count)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> spreadDis(-0.3f, 0.3f);
    std::uniform_real_distribution<float> speedDis(5.0f, 15.0f);
    std::uniform_real_distribution<float> sizeDis(0.2f, 0.6f);
    std::uniform_real_distribution<float> lifeDis(0.5f, 1.5f);

    for (int i = 0; i < count; i++)
    {
        if (particles.size() >= maxParticles)
            break;  // Don't exceed max particles

        Particle p;
        p.position = position;

        // Emit in opposite direction of thrust with some spread
        Vec3 emitDir = direction * -1.0f;
        emitDir = emitDir + Vec3(spreadDis(gen), spreadDis(gen), spreadDis(gen));
        emitDir = emitDir / emitDir.length();  // Normalize

        // Particle velocity = ship velocity + emission velocity
        float emitSpeed = speedDis(gen) * intensity;
        p.velocity = shipVelocity + emitDir * emitSpeed;

        p.size = sizeDis(gen) * intensity;
        p.maxLifetime = lifeDis(gen);
        p.lifetime = p.maxLifetime;
        p.alpha = 1.0f;

        // Engine exhaust colors: hot orange/yellow to cooler red
        float heatValue = std::uniform_real_distribution<float>(0.0f, 1.0f)(gen);
        if (heatValue > 0.7f)
        {
            // Hot core (yellow-white)
            p.color = Vec3(1.0f, 1.0f, 0.7f + heatValue * 0.3f);
        }
        else if (heatValue > 0.3f)
        {
            // Medium (orange)
            p.color = Vec3(1.0f, 0.5f + heatValue * 0.5f, 0.2f);
        }
        else
        {
            // Cool edges (red)
            p.color = Vec3(1.0f, 0.3f, 0.1f);
        }

        particles.push_back(p);
    }
}

void EngineParticles::update(float deltaTime)
{
    // Update existing particles
    for (auto& p : particles)
    {
        p.lifetime -= deltaTime;
        p.position = p.position + p.velocity * deltaTime;

        // Fade out as lifetime decreases
        p.alpha = p.lifetime / p.maxLifetime;

        // Particles cool down over time (fade toward red)
        float agePercent = 1.0f - (p.lifetime / p.maxLifetime);
        p.color.y = std::max(0.2f, p.color.y - agePercent * 0.5f * deltaTime);
        p.color.z = std::max(0.05f, p.color.z - agePercent * 0.8f * deltaTime);

        // Particles slow down due to "drag"
        p.velocity = p.velocity * (1.0f - deltaTime * 0.5f);

        // Particles shrink over time
        p.size = std::max(0.05f, p.size - deltaTime * 0.3f);
    }

    // Remove dead particles
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                      [](const Particle& p) { return p.lifetime <= 0.0f; }),
        particles.end()
    );

    updateBuffers();
}

void EngineParticles::render()
{
    if (particles.empty())
        return;

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(particles.size()));
    glBindVertexArray(0);
}

void EngineParticles::setupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Vertex layout: position(3), color(3), size(1), alpha(1) = 8 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void EngineParticles::updateBuffers()
{
    vertexData.clear();
    vertexData.reserve(particles.size() * 8);

    for (const auto& p : particles)
    {
        vertexData.push_back(p.position.x);
        vertexData.push_back(p.position.y);
        vertexData.push_back(p.position.z);
        vertexData.push_back(p.color.x);
        vertexData.push_back(p.color.y);
        vertexData.push_back(p.color.z);
        vertexData.push_back(p.size);
        vertexData.push_back(p.alpha);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexData.size() * sizeof(float),
                 vertexData.data(),
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
