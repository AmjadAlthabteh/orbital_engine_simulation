#include "EngineParticles.hpp"
#include <random>
#include <cmath>
#include <algorithm>

EngineParticles::EngineParticles(int maxCount) : maxParticles(maxCount)
{
    // OPTIMIZATION: Pre-allocate particle pool
    particles.resize(maxCount);
    for (auto& p : particles)
    {
        p.active = false;
    }
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

    // OPTIMIZATION: Reuse inactive particles from pool instead of allocating new ones
    int emitted = 0;
    for (size_t i = 0; i < particles.size() && emitted < count; i++)
    {
        if (!particles[i].active)
        {
            Particle& p = particles[i];
            p.active = true;
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

            emitted++;
        }
    }
}

void EngineParticles::update(float deltaTime)
{
    // OPTIMIZATION: Update only active particles, mark dead ones as inactive
    for (auto& p : particles)
    {
        if (!p.active)
            continue;

        p.lifetime -= deltaTime;

        // Mark dead particles as inactive for reuse
        if (p.lifetime <= 0.0f)
        {
            p.active = false;
            continue;
        }

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

    updateBuffers();
}

void EngineParticles::render()
{
    if (vertexData.empty())
        return;

    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(vertexData.size() / 8));
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

    // OPTIMIZATION: Only buffer active particles
    for (const auto& p : particles)
    {
        if (!p.active)
            continue;

        vertexData.push_back(p.position.x);
        vertexData.push_back(p.position.y);
        vertexData.push_back(p.position.z);
        vertexData.push_back(p.color.x);
        vertexData.push_back(p.color.y);
        vertexData.push_back(p.color.z);
        vertexData.push_back(p.size);
        vertexData.push_back(p.alpha);
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

// ============================================
// CRAZY PARTICLE EFFECTS!
// ============================================

void EngineParticles::emitRainbow(const Vec3& position, const Vec3& direction,
                                   const Vec3& shipVelocity, float colorPhase, int count)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> spreadDis(-0.3f, 0.3f);
    std::uniform_real_distribution<float> speedDis(5.0f, 15.0f);
    std::uniform_real_distribution<float> sizeDis(0.3f, 0.8f);
    std::uniform_real_distribution<float> lifeDis(0.7f, 1.8f);

    int emitted = 0;
    for (size_t i = 0; i < particles.size() && emitted < count; i++)
    {
        if (!particles[i].active)
        {
            Particle& p = particles[i];
            p.active = true;
            p.position = position;

            Vec3 emitDir = direction * -1.0f;
            emitDir = emitDir + Vec3(spreadDis(gen), spreadDis(gen), spreadDis(gen));
            emitDir = emitDir / emitDir.length();

            float emitSpeed = speedDis(gen);
            p.velocity = shipVelocity + emitDir * emitSpeed;

            p.size = sizeDis(gen);
            p.maxLifetime = lifeDis(gen);
            p.lifetime = p.maxLifetime;
            p.alpha = 1.0f;

            // RAINBOW COLOR based on phase + random offset
            float hue = colorPhase + std::uniform_real_distribution<float>(0.0f, 1.0f)(gen);
            while (hue > 6.28318f) hue -= 6.28318f;

            // HSV to RGB conversion for rainbow
            float c = 1.0f;
            float x = c * (1.0f - std::abs(std::fmod(hue / 1.047197f, 2.0f) - 1.0f));

            if (hue < 1.047197f)
                p.color = Vec3(c, x, 0);
            else if (hue < 2.094395f)
                p.color = Vec3(x, c, 0);
            else if (hue < 3.141593f)
                p.color = Vec3(0, c, x);
            else if (hue < 4.188790f)
                p.color = Vec3(0, x, c);
            else if (hue < 5.235988f)
                p.color = Vec3(x, 0, c);
            else
                p.color = Vec3(c, 0, x);

            emitted++;
        }
    }
}

void EngineParticles::emitSparks(const Vec3& position, int count)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> velDis(-8.0f, 8.0f);
    std::uniform_real_distribution<float> sizeDis(0.15f, 0.4f);
    std::uniform_real_distribution<float> lifeDis(0.3f, 0.8f);

    int emitted = 0;
    for (size_t i = 0; i < particles.size() && emitted < count; i++)
    {
        if (!particles[i].active)
        {
            Particle& p = particles[i];
            p.active = true;
            p.position = position;

            // Random velocity in all directions (sparks fly everywhere!)
            p.velocity = Vec3(velDis(gen), velDis(gen), velDis(gen));

            p.size = sizeDis(gen);
            p.maxLifetime = lifeDis(gen);
            p.lifetime = p.maxLifetime;
            p.alpha = 1.0f;

            // Bright white/yellow sparks
            p.color = Vec3(1.0f, 0.9f + velDis(gen) * 0.01f, 0.6f);

            emitted++;
        }
    }
}

void EngineParticles::emitWarpTrail(const Vec3& position, const Vec3& direction, int count)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> spreadDis(-0.5f, 0.5f);
    std::uniform_real_distribution<float> sizeDis(0.5f, 1.5f);
    std::uniform_real_distribution<float> lifeDis(0.2f, 0.6f);

    int emitted = 0;
    for (size_t i = 0; i < particles.size() && emitted < count; i++)
    {
        if (!particles[i].active)
        {
            Particle& p = particles[i];
            p.active = true;
            p.position = position;

            // Warp trail streaks backwards
            Vec3 emitDir = direction * -1.0f;
            emitDir = emitDir + Vec3(spreadDis(gen), spreadDis(gen), spreadDis(gen));
            p.velocity = emitDir * 30.0f;  // Fast streaks!

            p.size = sizeDis(gen);
            p.maxLifetime = lifeDis(gen);
            p.lifetime = p.maxLifetime;
            p.alpha = 1.0f;

            // Bright blue/cyan warp colors
            p.color = Vec3(0.3f, 0.7f + spreadDis(gen) * 0.2f, 1.0f);

            emitted++;
        }
    }
}

void EngineParticles::emitTurboFlames(const Vec3& position, const Vec3& direction,
                                       const Vec3& shipVelocity, int count)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> spreadDis(-0.2f, 0.2f);
    std::uniform_real_distribution<float> speedDis(10.0f, 25.0f);
    std::uniform_real_distribution<float> sizeDis(0.5f, 1.2f);
    std::uniform_real_distribution<float> lifeDis(0.4f, 1.2f);

    int emitted = 0;
    for (size_t i = 0; i < particles.size() && emitted < count; i++)
    {
        if (!particles[i].active)
        {
            Particle& p = particles[i];
            p.active = true;
            p.position = position;

            Vec3 emitDir = direction * -1.0f;
            emitDir = emitDir + Vec3(spreadDis(gen), spreadDis(gen), spreadDis(gen));
            emitDir = emitDir / emitDir.length();

            float emitSpeed = speedDis(gen);
            p.velocity = shipVelocity + emitDir * emitSpeed;

            p.size = sizeDis(gen);
            p.maxLifetime = lifeDis(gen);
            p.lifetime = p.maxLifetime;
            p.alpha = 1.0f;

            // INTENSE turbo colors: bright white, blue, and purple flames
            float intensity = std::uniform_real_distribution<float>(0.0f, 1.0f)(gen);
            if (intensity > 0.7f)
            {
                // White-hot core
                p.color = Vec3(1.0f, 1.0f, 1.0f);
            }
            else if (intensity > 0.4f)
            {
                // Blue flames
                p.color = Vec3(0.5f, 0.7f, 1.0f);
            }
            else
            {
                // Purple flames
                p.color = Vec3(0.8f, 0.4f, 1.0f);
            }

            emitted++;
        }
    }
}
