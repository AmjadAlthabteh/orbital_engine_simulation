#pragma once

// ===== ENHANCED PLANET SHADER WITH ATMOSPHERIC GLOW AND RIM LIGHTING =====
const char* enhancedPlanetVertexShader = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out vec3 ViewDir;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;

    // Calculate view direction in vertex shader for rim lighting
    vec3 viewPos = vec3(inverse(view) * vec4(0.0, 0.0, 0.0, 1.0));
    ViewDir = normalize(viewPos - FragPos);

    gl_Position = projection * view * worldPos;
}
)";

const char* enhancedPlanetFragmentShader = R"(
#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in vec3 ViewDir;

uniform vec3 objectColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool isSun;
uniform bool hasTexture;
uniform sampler2D planetTexture;
uniform bool hasAtmosphere;
uniform vec3 atmosphereColor;
uniform float atmosphereThickness;

void main()
{
    if (isSun)
    {
        // Sun with HDR glow
        vec3 glow = objectColor * 1.5;
        FragColor = vec4(glow, 1.0);
        return;
    }

    // Base color (texture or solid color)
    vec3 baseColor = objectColor;
    if (hasTexture)
    {
        baseColor = texture(planetTexture, TexCoord).rgb;
    }

    // Improved lighting model
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(ViewDir);

    // Ambient lighting (simulates scattered light)
    vec3 ambient = 0.15 * baseColor;

    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * baseColor;

    // Specular lighting (enhanced for planets with water/ice)
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = 0.4 * spec * vec3(1.0, 1.0, 1.0);

    // RIM LIGHTING (atmospheric scattering effect)
    float rimFactor = 1.0 - max(dot(viewDir, norm), 0.0);
    rimFactor = pow(rimFactor, 3.0);  // Sharper falloff

    vec3 rimColor = vec3(0.0);
    if (hasAtmosphere)
    {
        // Atmospheric rim glow (blue for Earth, orange for Mars, etc.)
        rimColor = atmosphereColor * rimFactor * atmosphereThickness;
    }
    else
    {
        // Subtle rim light even without atmosphere (light scattering)
        rimColor = baseColor * 0.5 * rimFactor * 0.3;
    }

    // Combine all lighting
    vec3 result = ambient + diffuse + specular + rimColor;

    // Add atmospheric glow on sunlit side
    if (hasAtmosphere && diff > 0.0)
    {
        float atmosphereGlow = pow(diff, 0.5) * atmosphereThickness;
        result += atmosphereColor * atmosphereGlow * 0.1;
    }

    FragColor = vec4(result, 1.0);
}
)";

// ===== NEBULA PARTICLE SHADER =====
const char* nebulaVertexShader = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aSize;
layout (location = 3) in float aDensity;
layout (location = 4) in float aRotation;

uniform mat4 view;
uniform mat4 projection;

out vec3 particleColor;
out float particleDensity;
out float particleRotation;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = aSize / (gl_Position.w * 0.1);  // Distance-based sizing
    particleColor = aColor;
    particleDensity = aDensity;
    particleRotation = aRotation;
}
)";

const char* nebulaFragmentShader = R"(
#version 460 core
out vec4 FragColor;

in vec3 particleColor;
in float particleDensity;
in float particleRotation;

void main()
{
    // Create soft, cloud-like nebula particles
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);

    // Soft falloff for cloud-like appearance
    float alpha = 1.0 - smoothstep(0.0, 0.5, dist);
    alpha = pow(alpha, 2.0) * particleDensity * 0.3;  // Soft, transparent clouds

    // Add some swirl/rotation effect
    float angle = atan(coord.y, coord.x) + particleRotation;
    float swirl = sin(angle * 4.0 + dist * 10.0) * 0.5 + 0.5;
    alpha *= (0.7 + swirl * 0.3);

    FragColor = vec4(particleColor, alpha);
}
)";

// ===== ENGINE PARTICLE SHADER =====
const char* engineParticleVertexShader = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aSize;
layout (location = 3) in float aAlpha;

uniform mat4 view;
uniform mat4 projection;

out vec3 particleColor;
out float particleAlpha;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = aSize * 100.0 / gl_Position.w;  // Distance-based sizing
    particleColor = aColor;
    particleAlpha = aAlpha;
}
)";

const char* engineParticleFragmentShader = R"(
#version 460 core
out vec4 FragColor;

in vec3 particleColor;
in float particleAlpha;

void main()
{
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);

    // Hot core with soft edges
    float alpha = 1.0 - smoothstep(0.2, 0.5, dist);
    alpha = pow(alpha, 1.5) * particleAlpha;

    // Additive blending for glow effect
    vec3 glowColor = particleColor * (1.0 + (1.0 - dist) * 0.5);

    FragColor = vec4(glowColor, alpha);
}
)";

// ===== ENHANCED STAR SHADER WITH BETTER GLOW =====
const char* enhancedStarVertexShader = R"(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aSize;

uniform mat4 view;
uniform mat4 projection;

out vec3 starColor;
out float starSize;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = aSize;
    starColor = aColor;
    starSize = aSize;
}
)";

const char* enhancedStarFragmentShader = R"(
#version 460 core
out vec4 FragColor;

in vec3 starColor;
in float starSize;

void main()
{
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);

    // Sharp core with glowing halo
    float core = 1.0 - smoothstep(0.0, 0.2, dist);
    float halo = 1.0 - smoothstep(0.2, 0.5, dist);

    float alpha = core + halo * 0.4;

    // Brighter center
    vec3 color = starColor * (1.0 + core * 0.5);

    // Add subtle star flare effect for larger stars
    if (starSize > 2.5)
    {
        float flare = abs(coord.x) < 0.02 || abs(coord.y) < 0.02 ? 0.6 : 0.0;
        alpha += flare;
    }

    FragColor = vec4(color, alpha);
}
)";

// ===== HUD SHADER (2D OVERLAY) =====
const char* hudVertexShader = R"(
#version 460 core
layout (location = 0) in vec2 aPos;

uniform vec2 position;
uniform vec2 scale;

void main()
{
    gl_Position = vec4(aPos * scale + position, 0.0, 1.0);
}
)";

const char* hudFragmentShader = R"(
#version 460 core
out vec4 FragColor;

uniform vec3 color;
uniform float alpha;

void main()
{
    FragColor = vec4(color, alpha);
}
)";
