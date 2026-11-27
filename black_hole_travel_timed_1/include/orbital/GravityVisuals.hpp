#pragma once

#include "Body.hpp"
#include "Shader.hpp"
#include <glad/glad.h>
#include <algorithm>
#include <cmath>

// Stretch along the strongest nearby radial field and compress the cross-section.
// Only the render matrix changes, so lighting and atmosphere shells follow the mesh.
inline Mat4 tidalModel(Mat4 model, const Body& body, const Body* sun, const Body* blackHole, float time)
{
    Vec3 axis;
    float stretch = 0.0f;
    for (const Body* source : {sun, blackHole})
    {
        if (!source || source == &body)
            continue;
        const Vec3 offset = source->position - body.position;
        const float distance = offset.length();
        const bool isBlackHole = source == blackHole;
        const float range = source->radius * (isBlackHole ? 12.0f : 3.0f);
        if (distance <= 0.001f || distance >= range)
            continue;

        const Vec3 direction = offset / distance;
        const float proximity = 1.0f - distance / range;
        const float closingSpeed = (body.velocity - source->velocity).dot(direction);
        const float approach = std::clamp(closingSpeed / 8.0f, 0.0f, 1.0f);
        const float amount = proximity * proximity * (0.78f + 0.22f * approach)
                           * (isBlackHole ? 1.2f : 0.28f);
        if (amount > stretch)
        {
            stretch = amount;
            axis = direction;
        }
    }
    if (stretch == 0.0f)
        return model;

    stretch *= 0.94f + 0.06f * std::sin(time * 2.8f);
    const float compression = 1.0f / std::sqrt(1.0f + stretch);
    for (int column = 0; column < 12; column += 4)
    {
        const Vec3 basis(model.m[column], model.m[column + 1], model.m[column + 2]);
        const Vec3 deformed = basis * compression
                            + axis * (basis.dot(axis) * (1.0f + stretch - compression));
        model.m[column] = deformed.x;
        model.m[column + 1] = deformed.y;
        model.m[column + 2] = deformed.z;
    }
    return model;
}

class BlackHoleInflow
{
public:
    BlackHoleInflow() { glGenVertexArrays(1, &vao); }
    ~BlackHoleInflow() { glDeleteVertexArrays(1, &vao); }
    BlackHoleInflow(const BlackHoleInflow&) = delete;
    BlackHoleInflow& operator=(const BlackHoleInflow&) = delete;

    // Called during the additive black-hole pass. Motion is generated on the GPU.
    void render(const Body& blackHole, const Mat4& view, const Mat4& projection, float time) const
    {
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("center", blackHole.position);
        shader.setFloat("holeRadius", blackHole.radius);
        shader.setFloat("time", time);
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, 192);
        glBindVertexArray(0);
    }

private:
    static constexpr const char* vertex = R"(
#version 330 core
uniform mat4 view;
uniform mat4 projection;
uniform vec3 center;
uniform float holeRadius;
uniform float time;
out vec4 tint;

void main()
{
    float seed = float(gl_VertexID);
    float phase = fract(seed * 0.618034);
    float spread = fract(seed * 0.754878);
    float age = fract(phase + time * mix(0.17, 0.29, spread));
    float angle = seed * 2.399963 + age * (3.0 + 10.0 * age);
    float radius = holeRadius * mix(5.5 + spread, 1.02, age);
    float height = holeRadius * (spread - 0.5) * (1.0 - age);
    vec3 position = center + vec3(cos(angle) * radius, height, sin(angle) * radius);
    gl_Position = projection * view * vec4(position, 1.0);
    gl_PointSize = mix(1.0, 2.6, age) + spread;
    float fade = smoothstep(0.0, 0.16, age) * (1.0 - smoothstep(0.84, 1.0, age));
    tint = vec4(mix(vec3(0.43, 0.62, 1.0), vec3(1.0, 0.54, 0.23), age), fade * 0.48);
}
)";
    static constexpr const char* fragment = R"(
#version 330 core
in vec4 tint;
out vec4 color;

void main()
{
    float glow = 1.0 - smoothstep(0.05, 0.5, length(gl_PointCoord - vec2(0.5)));
    color = vec4(tint.rgb, tint.a * glow);
}
)";

    Shader shader{vertex, fragment};
    GLuint vao = 0;
};
