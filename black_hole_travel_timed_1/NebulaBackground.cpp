#include "NebulaBackground.hpp"
#include <random>
#include <cmath>

NebulaBackground::NebulaBackground()
{
    generateNebula();
    setupBuffers();
}

NebulaBackground::~NebulaBackground()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void NebulaBackground::generateNebula()
{
    nebulaParticles.clear();
    regions.clear();

    // Create multiple nebula regions with different colors
    // Region 1: Purple/magenta nebula (left side)
    createNebulaRegion(Vec3(-200.0f, 100.0f, -150.0f), 180.0f,
                       Vec3(0.8f, 0.2f, 0.9f), 800);

    // Region 2: Blue/cyan nebula (right side)
    createNebulaRegion(Vec3(220.0f, -80.0f, -180.0f), 160.0f,
                       Vec3(0.2f, 0.5f, 1.0f), 700);

    // Region 3: Orange/red nebula (top)
    createNebulaRegion(Vec3(0.0f, 250.0f, -200.0f), 140.0f,
                       Vec3(1.0f, 0.4f, 0.2f), 600);

    // Region 4: Green/teal nebula (bottom)
    createNebulaRegion(Vec3(100.0f, -200.0f, -170.0f), 120.0f,
                       Vec3(0.2f, 0.9f, 0.6f), 500);

    // Region 5: Yellow/gold nebula (center-back)
    createNebulaRegion(Vec3(-50.0f, 0.0f, -300.0f), 200.0f,
                       Vec3(1.0f, 0.8f, 0.3f), 900);

    // Region 6: Deep space nebula (far background, darker)
    createNebulaRegion(Vec3(150.0f, 150.0f, -400.0f), 250.0f,
                       Vec3(0.4f, 0.2f, 0.6f), 1000);

    // Add some small nebula wisps scattered around
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> posDis(-300.0f, 300.0f);
    std::uniform_real_distribution<float> colorDis(0.2f, 0.8f);

    for (int i = 0; i < 10; i++)
    {
        Vec3 wispColor(colorDis(gen), colorDis(gen), colorDis(gen));
        createNebulaRegion(Vec3(posDis(gen), posDis(gen), -posDis(gen)),
                           50.0f + colorDis(gen) * 50.0f, wispColor, 200);
    }

    updateBuffers();
}

void NebulaBackground::createNebulaRegion(const Vec3& center, float radius,
                                          const Vec3& color, int particleCount)
{
    NebulaRegion region;
    region.center = center;
    region.radius = radius;
    region.color = color;
    region.particleCount = particleCount;
    regions.push_back(region);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    std::uniform_real_distribution<float> densityDis(0.1f, 1.0f);
    std::uniform_real_distribution<float> sizeDis(10.0f, 40.0f);
    std::uniform_real_distribution<float> rotDis(0.0f, 6.28318f);

    for (int i = 0; i < particleCount; i++)
    {
        NebulaParticle particle;

        // Random position within sphere (using rejection sampling for uniform distribution)
        Vec3 offset;
        float distSq;
        do
        {
            offset = Vec3(dis(gen), dis(gen), dis(gen));
            distSq = offset.x * offset.x + offset.y * offset.y + offset.z * offset.z;
        } while (distSq > 1.0f);

        // Scale to nebula radius with falloff toward edges
        float dist = std::sqrt(distSq);
        float falloff = 1.0f - (dist * dist);  // Density falls off toward edges

        particle.position = center + offset * radius;
        particle.density = densityDis(gen) * falloff;
        particle.size = sizeDis(gen) * (1.0f + falloff * 0.5f);  // Larger in center
        particle.rotation = rotDis(gen);

        // Color variation within region
        float colorVar = dis(gen) * 0.2f;
        particle.color = Vec3(
            std::max(0.0f, std::min(1.0f, color.x + colorVar)),
            std::max(0.0f, std::min(1.0f, color.y + colorVar)),
            std::max(0.0f, std::min(1.0f, color.z + colorVar))
        );

        nebulaParticles.push_back(particle);
    }
}

void NebulaBackground::setupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Vertex layout: position(3), color(3), size(1), density(1), rotation(1) = 9 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void NebulaBackground::updateBuffers()
{
    vertexData.clear();
    vertexData.reserve(nebulaParticles.size() * 9);

    for (const auto& particle : nebulaParticles)
    {
        vertexData.push_back(particle.position.x);
        vertexData.push_back(particle.position.y);
        vertexData.push_back(particle.position.z);
        vertexData.push_back(particle.color.x);
        vertexData.push_back(particle.color.y);
        vertexData.push_back(particle.color.z);
        vertexData.push_back(particle.size);
        vertexData.push_back(particle.density);
        vertexData.push_back(particle.rotation);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertexData.size() * sizeof(float),
                 vertexData.data(),
                 GL_DYNAMIC_DRAW);  // Dynamic since we animate
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void NebulaBackground::update(float deltaTime)
{
    // Slowly rotate and pulse nebula particles
    static float time = 0.0f;
    time += deltaTime * 0.1f;  // Slow animation

    for (size_t i = 0; i < nebulaParticles.size(); i++)
    {
        auto& particle = nebulaParticles[i];

        // Gentle pulsing effect
        float pulseFreq = 0.5f + (i % 100) * 0.01f;
        float pulse = std::sin(time * pulseFreq + particle.rotation);
        particle.density = std::max(0.05f, std::min(1.0f,
            particle.density + pulse * 0.002f));

        // Very slow rotation
        particle.rotation += deltaTime * 0.02f;
        if (particle.rotation > 6.28318f)
            particle.rotation -= 6.28318f;
    }

    // Update every 10 frames for performance
    static int updateCounter = 0;
    if (++updateCounter > 10)
    {
        updateBuffers();
        updateCounter = 0;
    }
}

void NebulaBackground::render()
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(nebulaParticles.size()));
    glBindVertexArray(0);
}
