#pragma once
#include "CelestialBody.hpp"
#include "Shader.hpp"
#include "Sphere.hpp"
#include "Mat4.hpp"
#include "Vec3.hpp"
#include <glad/glad.h>

// Gravitational lensing shader for black hole
const char* lensingVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const char* lensingFragmentShader = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform vec3 blackHolePos;
uniform float blackHoleMass;
uniform float schwarzschildRadius;
uniform sampler2D skyboxTexture;  // Background space texture
uniform float time;

void main()
{
    vec3 viewDir = normalize(FragPos - viewPos);
    vec3 toBlackHole = blackHolePos - FragPos;
    float distToBlackHole = length(toBlackHole);
    float silhouette = max(dot(normalize(Normal), normalize(viewPos - FragPos)), 0.0);
    float limb = 1.0 - silhouette;

    // Calculate gravitational lensing effect
    // Bend light rays based on distance to event horizon
    float lensStrength = (schwarzschildRadius * schwarzschildRadius) / (distToBlackHole * distToBlackHole);
    lensStrength = min(lensStrength, 1.0);

    // Bend the view direction toward the black hole
    vec3 bentDir = normalize(viewDir + normalize(toBlackHole) * lensStrength * 0.5);

    // A narrow photon ring appears at the projected limb of the lensing shell.
    float photonRing = exp(-pow((limb - 0.72) * 19.0, 2.0));
    float secondaryRing = exp(-pow((limb - 0.88) * 34.0, 2.0)) * 0.35;
    float shimmer = 0.88 + 0.12 * sin(time * 1.3 + TexCoords.x * 90.0);
    vec3 ringColor = mix(vec3(1.0, 0.34, 0.06), vec3(0.72, 0.84, 1.0), limb);
    float alpha = (photonRing + secondaryRing) * shimmer * 0.72;
    FragColor = vec4(ringColor * alpha * 2.4, alpha);
}
)";

class BlackHole : public CelestialBody
{
private:
    float schwarzschildRadius;  // Event horizon radius
    float photonSphereRadius;   // Where light orbits
    Sphere eventHorizonSphere;
    Sphere lensingEffectSphere;

public:
    BlackHole(const std::string& name, float mass, float radius, const Vec3& color)
        : CelestialBody(name, mass, radius, color)
    {
        // Calculate Schwarzschild radius (event horizon)
        // rs = 2GM/c^2, but we'll use simplified physics
        schwarzschildRadius = radius * 0.8f;  // Event horizon smaller than visual radius
        photonSphereRadius = schwarzschildRadius * 1.5f;  // Photon sphere at 1.5x Schwarzschild

        // Create spheres for rendering
        eventHorizonSphere = Sphere(20, 20);  // Lower detail for event horizon
        lensingEffectSphere = Sphere(40, 40);  // Higher detail for lensing effect
    }

    void renderLensingEffect(Shader* lensingShader, const Mat4& view, const Mat4& projection, const Vec3& cameraPos)
    {
        lensingShader->use();

        Vec3 pos = getPhysicsBody().position;
        float rad = getRadius();

        // Render lensing effect sphere (larger than event horizon)
        Mat4 lensingModel = Mat4::translation(pos.x, pos.y, pos.z) * Mat4::scale(rad * 3.0f);

        lensingShader->setMat4("model", lensingModel);
        lensingShader->setMat4("view", view);
        lensingShader->setMat4("projection", projection);
        lensingShader->setVec3("viewPos", cameraPos);
        lensingShader->setVec3("blackHolePos", pos);
        lensingShader->setFloat("blackHoleMass", getPhysicsBody().mass);
        lensingShader->setFloat("schwarzschildRadius", schwarzschildRadius);

        // Render with transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        lensingEffectSphere.draw();

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }

    void renderEventHorizon(Shader* shader, const Mat4& view, const Mat4& projection)
    {
        shader->use();

        Vec3 pos = getPhysicsBody().position;

        // Render pure black event horizon
        Mat4 horizonModel = Mat4::translation(pos.x, pos.y, pos.z) * Mat4::scale(schwarzschildRadius);

        shader->setMat4("model", horizonModel);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);
        shader->setVec3("objectColor", Vec3(0.0f, 0.0f, 0.0f));
        shader->setBool("isSun", false);
        shader->setBool("hasAtmosphere", false);

        eventHorizonSphere.draw();
    }

    float getSchwarzchildRadius() const { return schwarzschildRadius; }
    float getPhotonSphereRadius() const { return photonSphereRadius; }
};
