#include "GalaxyBackground.hpp"
#include <random>
#include <cmath>

GalaxyBackground::GalaxyBackground(int starCount)
    : VAO(0), VBO(0), animationTime(0.0f)
{
    generateGalaxy(starCount);
    setupBuffers();
}

GalaxyBackground::~GalaxyBackground()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void GalaxyBackground::generateGalaxy(int starCount)
{
    stars.clear();
    stars.reserve(starCount + 5000);  // Extra for core, halo, and distant galaxies

    // Create realistic galactic structure
    generateSpiralArm(starCount / 4, 0.0f, 30.0f);       // Main spiral arm 1
    generateSpiralArm(starCount / 4, 3.14159f, 30.0f);   // Main spiral arm 2 (opposite)
    generateSpiralArm(starCount / 6, 1.57f, 25.0f);      // Secondary arm 1
    generateSpiralArm(starCount / 6, 4.71f, 25.0f);      // Secondary arm 2
    generateGalacticCore(starCount / 8);                  // Dense galactic core
    generateHaloStars(starCount / 8);                     // Sparse halo around galaxy
    generateDistantGalaxies(100);                         // Tiny distant galaxies

    updateBuffers();
}

void GalaxyBackground::generateSpiralArm(int starCount, float armAngle, float armWidth)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    std::normal_distribution<float> normalDis(0.0f, 1.0f);

    for (int i = 0; i < starCount; i++)
    {
        GalaxyStar star;

        // Logarithmic spiral pattern: r = a * e^(b*theta)
        float spiralTightness = 0.15f;
        float theta = armAngle + dis(gen) * 6.28318f * 2.0f;  // Multiple wraps
        float radius = 100.0f + 200.0f * std::exp(spiralTightness * (theta - armAngle));

        // Add some randomness perpendicular to spiral
        float armOffset = normalDis(gen) * armWidth;

        // Convert to Cartesian coordinates
        float x = radius * std::cos(theta) + armOffset * std::cos(theta + 1.57f);
        float z = radius * std::sin(theta) + armOffset * std::sin(theta + 1.57f);
        float y = normalDis(gen) * 20.0f;  // Thin disk

        star.position = Vec3(x, y, z);

        // Realistic star colors based on temperature
        float temp = dis(gen);
        if (temp < 0.1f)  // Hot blue giants (10%)
        {
            star.color = Vec3(0.7f, 0.8f, 1.0f);
            star.size = 2.5f + dis(gen) * 3.0f;
            star.brightness = 0.9f + dis(gen) * 0.1f;
        }
        else if (temp < 0.3f)  // Blue-white stars (20%)
        {
            star.color = Vec3(0.85f, 0.9f, 1.0f);
            star.size = 1.8f + dis(gen) * 2.0f;
            star.brightness = 0.8f + dis(gen) * 0.2f;
        }
        else if (temp < 0.7f)  // White/yellow stars like Sun (40%)
        {
            star.color = Vec3(1.0f, 0.95f, 0.85f);
            star.size = 1.2f + dis(gen) * 1.5f;
            star.brightness = 0.6f + dis(gen) * 0.3f;
        }
        else if (temp < 0.9f)  // Orange stars (20%)
        {
            star.color = Vec3(1.0f, 0.7f, 0.5f);
            star.size = 1.0f + dis(gen) * 1.2f;
            star.brightness = 0.5f + dis(gen) * 0.3f;
        }
        else  // Red dwarfs (10%)
        {
            star.color = Vec3(1.0f, 0.5f, 0.3f);
            star.size = 0.8f + dis(gen) * 0.8f;
            star.brightness = 0.4f + dis(gen) * 0.4f;
        }

        star.twinkleSpeed = 0.5f + dis(gen) * 2.0f;
        star.twinklePhase = dis(gen) * 6.28318f;

        stars.push_back(star);
    }
}

void GalaxyBackground::generateGalacticCore(int starCount)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    std::uniform_real_distribution<float> radiusDis(0.0f, 80.0f);

    for (int i = 0; i < starCount; i++)
    {
        GalaxyStar star;

        // Dense spherical core
        float theta = dis(gen) * 6.28318f;
        float phi = std::acos(2.0f * dis(gen) - 1.0f);
        float radius = radiusDis(gen) * std::pow(dis(gen), 0.3f);  // Denser toward center

        star.position.x = radius * std::sin(phi) * std::cos(theta);
        star.position.y = radius * std::sin(phi) * std::sin(theta) * 0.3f;  // Flatten slightly
        star.position.z = radius * std::cos(phi);

        // Core stars are predominantly old yellow/red stars
        float temp = dis(gen);
        if (temp < 0.6f)
        {
            star.color = Vec3(1.0f, 0.85f, 0.6f);  // Yellow
        }
        else
        {
            star.color = Vec3(1.0f, 0.6f, 0.4f);   // Orange-red
        }

        star.size = 1.5f + dis(gen) * 2.0f;
        star.brightness = 0.7f + dis(gen) * 0.3f;
        star.twinkleSpeed = 0.3f + dis(gen) * 1.0f;
        star.twinklePhase = dis(gen) * 6.28318f;

        stars.push_back(star);
    }
}

void GalaxyBackground::generateHaloStars(int starCount)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    for (int i = 0; i < starCount; i++)
    {
        GalaxyStar star;

        // Sparse spherical halo around galaxy
        float theta = dis(gen) * 6.28318f;
        float phi = std::acos(2.0f * dis(gen) - 1.0f);
        float radius = 300.0f + dis(gen) * 500.0f;

        star.position.x = radius * std::sin(phi) * std::cos(theta);
        star.position.y = radius * std::sin(phi) * std::sin(theta);
        star.position.z = radius * std::cos(phi);

        // Halo stars are old, dim, reddish
        star.color = Vec3(0.9f, 0.7f, 0.5f);
        star.size = 0.6f + dis(gen) * 0.8f;
        star.brightness = 0.3f + dis(gen) * 0.3f;
        star.twinkleSpeed = 0.2f + dis(gen) * 0.5f;
        star.twinklePhase = dis(gen) * 6.28318f;

        stars.push_back(star);
    }
}

void GalaxyBackground::generateDistantGalaxies(int count)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    for (int i = 0; i < count; i++)
    {
        GalaxyStar star;  // Actually a tiny distant galaxy

        // Very far away
        float theta = dis(gen) * 6.28318f;
        float phi = std::acos(2.0f * dis(gen) - 1.0f);
        float radius = 800.0f + dis(gen) * 400.0f;

        star.position.x = radius * std::sin(phi) * std::cos(theta);
        star.position.y = radius * std::sin(phi) * std::sin(theta);
        star.position.z = radius * std::cos(phi);

        // Faint, fuzzy appearance
        star.color = Vec3(0.8f, 0.8f, 0.9f);
        star.size = 3.0f + dis(gen) * 4.0f;  // Large but very faint
        star.brightness = 0.1f + dis(gen) * 0.15f;
        star.twinkleSpeed = 0.1f + dis(gen) * 0.2f;
        star.twinklePhase = dis(gen) * 6.28318f;

        stars.push_back(star);
    }
}

void GalaxyBackground::setupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Vertex layout: position(3), color(3), size(1) = 7 floats
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GalaxyBackground::updateBuffers()
{
    vertexData.clear();
    vertexData.reserve(stars.size() * 7);

    for (const auto& star : stars)
    {
        vertexData.push_back(star.position.x);
        vertexData.push_back(star.position.y);
        vertexData.push_back(star.position.z);
        vertexData.push_back(star.color.x * star.brightness);
        vertexData.push_back(star.color.y * star.brightness);
        vertexData.push_back(star.color.z * star.brightness);
        vertexData.push_back(star.size);
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertexData.size() * sizeof(float),
        vertexData.data(),
        GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GalaxyBackground::update(float deltaTime)
{
    animationTime += deltaTime;

    // Realistic star twinkling (caused by atmospheric effects)
    for (auto& star : stars)
    {
        float twinkle = std::sin(animationTime * star.twinkleSpeed + star.twinklePhase);
        star.brightness = 0.3f + (0.7f * (twinkle * 0.5f + 0.5f));
        star.brightness = std::max(0.2f, std::min(1.0f, star.brightness));
    }

    // Update every few frames for performance
    static int updateCounter = 0;
    if (++updateCounter > 3)
    {
        updateBuffers();
        updateCounter = 0;
    }
}

void GalaxyBackground::render()
{
    glBindVertexArray(VAO);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(stars.size()));
    glBindVertexArray(0);
}

const std::vector<GalaxyStar>& GalaxyBackground::getStars() const
{
    return stars;
}
