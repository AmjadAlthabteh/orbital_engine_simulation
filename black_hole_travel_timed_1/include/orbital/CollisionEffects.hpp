#pragma once

#include "PhysicsEngine.hpp"
#include "Shader.hpp"

#include <glad/glad.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <random>
#include <vector>

class CollisionEffects
{
public:
    CollisionEffects()
    {
        impacts.reserve(maxImpacts);
        vertices.reserve(maxImpacts * (maxDebris + 2) * 6);
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                              reinterpret_cast<const void*>(offsetof(Vertex, radius)));
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    ~CollisionEffects()
    {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &vao);
    }

    CollisionEffects(const CollisionEffects&) = delete;
    CollisionEffects& operator=(const CollisionEffects&) = delete;

    float update(const std::vector<CollisionEvent>& collisions, float dt, const Vec3& cameraPosition)
    {
        for (auto& impact : impacts)
            impact.age += dt;
        impacts.erase(std::remove_if(impacts.begin(), impacts.end(), [](const Impact& impact) {
            return impact.age >= maxLifetime;
        }), impacts.end());

        float shake = 0.0f;
        for (const auto& collision : collisions)
        {
            if (collision.sequence <= lastSequence)
                continue;
            lastSequence = collision.sequence;

            const double massA = collision.bodyA->mass;
            const double massB = collision.bodyB->mass;
            const double closingSpeed = collision.intensity;
            if (massA <= 0.0 || massB <= 0.0 || closingSpeed <= 0.0 || !collision.position.isFinite())
                continue;

            // Energy in the center-of-mass frame along the contact normal.
            const double reducedMass = massA * massB / (massA + massB);
            const double energy = 0.5 * reducedMass * closingSpeed * closingSpeed;
            if (!std::isfinite(energy))
                continue;

            // Logarithmic scaling keeps both small contacts and giant impacts readable.
            const float strength = static_cast<float>(std::min(1.0, std::log1p(energy) / 16.0));
            const float radius = 1.5f + 14.0f * strength;
            if (impacts.size() == maxImpacts)
                impacts.erase(impacts.begin());
            impacts.push_back({collision.position, radius, strength, 0.0f, {}});

            auto& debris = impacts.back().debris;
            const int count = 10 + static_cast<int>((maxDebris - 10) * strength);
            debris.reserve(count);
            for (int i = 0; i < count; ++i)
            {
                const float z = randomUnit(rng) * 2.0f - 1.0f;
                const float angle = randomUnit(rng) * 6.2831853f;
                const float ring = std::sqrt(std::max(0.0f, 1.0f - z * z));
                const Vec3 direction(ring * std::cos(angle), ring * std::sin(angle), z);
                debris.push_back({direction * (radius * (0.55f + 0.9f * randomUnit(rng))),
                                  0.04f + 0.15f * randomUnit(rng) * (0.5f + strength),
                                  0.45f + (maxLifetime - 0.45f) * randomUnit(rng)});
            }

            const float distance = (collision.position - cameraPosition).length();
            shake = std::max(shake, (0.08f + 0.32f * strength) / (1.0f + distance / 30.0f));
        }
        return shake;
    }

    void render(const Mat4& view, const Mat4& projection)
    {
        if (impacts.empty())
            return;

        vertices.clear();
        for (const auto& impact : impacts)
        {
            const float wave = impact.age / 0.95f;
            if (wave < 1.0f)
            {
                addSprite(impact.position, impact.radius * (0.12f + 0.88f * wave),
                          (0.35f + 0.45f * impact.strength) * (1.0f - wave), wave, 0.0f);
            }

            const float flash = impact.age / 0.16f;
            if (flash < 1.0f)
            {
                const float fade = 1.0f - flash;
                addSprite(impact.position, impact.radius * (0.18f + 0.17f * flash),
                          (0.6f + 0.4f * impact.strength) * fade * fade, flash, 1.0f);
            }

            for (const auto& particle : impact.debris)
            {
                const float progress = impact.age / particle.lifetime;
                if (progress >= 1.0f)
                    continue;

                const float fade = 1.0f - progress;
                const float travel = impact.age / (1.0f + impact.age * 0.6f);
                addSprite(impact.position + particle.velocity * travel, particle.radius,
                          (0.55f + 0.45f * impact.strength) * fade * fade, progress, 2.0f);
            }
        }
        if (vertices.empty())
            return;

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_TRUE);
    }

private:
    static constexpr size_t maxImpacts = 12;
    static constexpr int maxDebris = 96;
    static constexpr float maxLifetime = 1.7f;

    struct Debris
    {
        Vec3 velocity;
        float radius;
        float lifetime;
    };

    struct Impact
    {
        Vec3 position;
        float radius;
        float strength;
        float age;
        std::vector<Debris> debris;
    };

    struct Vertex
    {
        Vec3 position;
        float radius;
        float opacity;
        float progress;
        float kind;
    };

    void addSprite(const Vec3& position, float radius, float opacity, float progress, float kind)
    {
        vertices.insert(vertices.end(), 6, Vertex{position, radius, opacity, progress, kind});
    }

    // Camera-facing quads avoid the hardware point-size limit on large shockwaves.
    static constexpr const char* vertexSource = R"(
#version 330 core
layout(location = 0) in vec3 center;
layout(location = 1) in vec4 effect;
uniform mat4 view;
uniform mat4 projection;
out vec2 uv;
flat out vec3 style;

const vec2 corners[6] = vec2[6](
    vec2(-1, -1), vec2(1, -1), vec2(1, 1),
    vec2(-1, -1), vec2(1, 1), vec2(-1, 1)
);

void main()
{
    uv = corners[gl_VertexID % 6];
    style = effect.yzw;
    vec4 cameraSpace = view * vec4(center, 1.0);
    cameraSpace.xy += uv * effect.x;
    gl_Position = projection * cameraSpace;
}
)";

    static constexpr const char* fragmentSource = R"(
#version 330 core
in vec2 uv;
flat in vec3 style;
out vec4 color;

void main()
{
    float radius = length(uv);
    float progress = style.y;
    vec3 tint;
    float shape;
    if (style.z < 0.5)
    {
        float edge = abs(radius - 0.81);
        shape = 1.0 - smoothstep(0.015, 0.13, edge);
        shape += 0.22 * (1.0 - smoothstep(0.02, 0.24, abs(radius - 0.69)));
        tint = mix(vec3(1.8, 1.2, 0.72), vec3(0.35, 0.58, 1.0), progress);
    }
    else
    {
        float glow = max(0.0, 1.0 - radius);
        shape = glow * glow;
        tint = style.z < 1.5
            ? vec3(3.4, 2.6, 1.55)
            : mix(vec3(2.8, 1.75, 0.65), vec3(0.60, 0.09, 0.025), sqrt(progress));
    }
    color = vec4(tint, style.x * shape);
}
)";

    Shader shader{vertexSource, fragmentSource};
    GLuint vao = 0;
    GLuint vbo = 0;
    std::uint64_t lastSequence = 0;
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<float> randomUnit{0.0f, 1.0f};
    std::vector<Impact> impacts;
    std::vector<Vertex> vertices;
};
