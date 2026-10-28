#pragma once

// ===== ENHANCED PLANET SHADER WITH ATMOSPHERIC GLOW AND RIM LIGHTING =====
const char* enhancedPlanetVertexShader = R"(
#version 330 core
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
#version 330 core
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
uniform float roughness;
uniform float time;
uniform int materialType;
uniform float lightIntensity;

float hash31(vec3 p)
{
    p = fract(p * 0.1031);
    p += dot(p, p.yzx + 33.33);
    return fract((p.x + p.y) * p.z);
}

float surfaceNoise(vec3 p)
{
    float n = 0.0;
    float amplitude = 0.5;
    for (int i = 0; i < 4; ++i)
    {
        n += hash31(floor(p)) * amplitude;
        p *= 2.03;
        amplitude *= 0.5;
    }
    return n;
}

void main()
{
    if (isSun)
    {
        vec3 norm = normalize(Normal);
        vec3 viewDir = normalize(ViewDir);
        float limb = clamp(dot(norm, viewDir), 0.0, 1.0);
        float granulation = surfaceNoise(norm * 38.0 + vec3(time * 0.035));
        float cells = smoothstep(0.28, 0.78, granulation);
        vec3 hot = vec3(1.0, 0.91, 0.58);
        vec3 warm = vec3(1.0, 0.30, 0.035);
        vec3 solar = mix(warm, hot, cells * 0.65 + limb * 0.35);
        solar *= 3.2 + cells * 1.3;
        // Natural limb darkening keeps the star spherical instead of flat.
        solar *= 0.62 + 0.38 * pow(limb, 0.35);
        FragColor = vec4(solar, 1.0);
        return;
    }

    // The event horizon absorbs light rather than behaving like a dark planet.
    if (materialType == 9)
    {
        FragColor = vec4(vec3(0.00005), 1.0);
        return;
    }

    // Base color (texture or solid color)
    vec3 baseColor = objectColor;
    if (hasTexture)
    {
        baseColor = texture(planetTexture, TexCoord).rgb;
    }

    // Independent high cloud layer for Earth. Advection keeps weather visibly
    // separate from the rotating terrain texture.
    float cloudMask = 0.0;
    if (materialType == 1)
    {
        vec2 cloudUV = TexCoord + vec2(time * 0.0015, 0.0);
        float cloudNoise = surfaceNoise(vec3(cloudUV * vec2(32.0, 18.0), 4.7));
        cloudMask = smoothstep(0.63, 0.79, cloudNoise) *
                    smoothstep(0.98, 0.72, abs(TexCoord.y * 2.0 - 1.0));
        baseColor = mix(baseColor, vec3(0.93, 0.96, 1.0), cloudMask * 0.62);
    }

    // Improved lighting model
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(ViewDir);

    float nDotL = max(dot(norm, lightDir), 0.0);
    vec3 halfVector = normalize(lightDir + viewDir);
    float nDotH = max(dot(norm, halfVector), 0.0);
    float nDotV = max(dot(norm, viewDir), 0.001);

    // A compact energy-conserving microfacet highlight. Rough rock stays matte,
    // while oceans, ice and clouds catch a small sun glint.
    float alpha = max(roughness * roughness, 0.025);
    float alpha2 = alpha * alpha;
    float denom = nDotH * nDotH * (alpha2 - 1.0) + 1.0;
    float distribution = alpha2 / max(3.14159265 * denom * denom, 0.001);
    float k = (roughness + 1.0) * (roughness + 1.0) / 8.0;
    float geometryV = nDotV / (nDotV * (1.0 - k) + k);
    float geometryL = nDotL / (nDotL * (1.0 - k) + k);
    float fresnel = 0.04 + 0.96 * pow(1.0 - max(dot(halfVector, viewDir), 0.0), 5.0);
    vec3 specular = vec3(distribution * geometryV * geometryL * fresnel /
                         max(4.0 * nDotV * nDotL, 0.001));

    float nightFill = hasAtmosphere ? 0.018 : 0.008;
    vec3 ambient = baseColor * nightFill;
    vec3 diffuse = baseColor * nDotL * (1.0 - fresnel) * lightIntensity / 3.14159265;

    // RIM LIGHTING (atmospheric scattering effect)
    float rimFactor = pow(1.0 - nDotV, 4.0);

    vec3 rimColor = vec3(0.0);
    if (hasAtmosphere)
    {
        // Atmospheric rim glow (blue for Earth, orange for Mars, etc.)
        // Scattering is strongest at the sunlit limb and softly crosses the terminator.
        float sunward = smoothstep(-0.18, 0.35, dot(norm, lightDir));
        rimColor = atmosphereColor * rimFactor * atmosphereThickness * sunward * 1.35;
    }
    else
    {
        // Subtle rim light even without atmosphere (light scattering)
        rimColor = baseColor * rimFactor * 0.035;
    }

    // Combine all lighting
    vec3 result = ambient + diffuse + specular * nDotL * lightIntensity + rimColor;

    // Sparse warm settlement lights, visible only on Earth's land and night side.
    if (materialType == 1)
    {
        float land = smoothstep(0.04, 0.20, baseColor.g - baseColor.b * 0.52);
        float cityPattern = surfaceNoise(vec3(TexCoord * vec2(220.0, 110.0), 13.0));
        float cities = smoothstep(0.86, 0.94, cityPattern) * land * (1.0 - cloudMask);
        float night = smoothstep(0.05, -0.32, dot(norm, lightDir));
        result += vec3(1.0, 0.42, 0.10) * cities * night * 1.8;
    }

    // Add atmospheric glow on sunlit side
    if (hasAtmosphere && diff > 0.0)
    {
        float forwardScatter = pow(max(dot(viewDir, -lightDir), 0.0), 8.0);
        result += atmosphereColor * forwardScatter * atmosphereThickness * 0.08 * nDotL;
    }

    FragColor = vec4(result, 1.0);
}
)";

// ===== VOLUMETRIC ATMOSPHERE SHELL =====
const char* atmosphereVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 WorldPos;
out vec3 WorldNormal;

void main()
{
    vec4 world = model * vec4(aPos, 1.0);
    WorldPos = world.xyz;
    WorldNormal = normalize(mat3(transpose(inverse(model))) * aNormal);
    gl_Position = projection * view * world;
}
)";

const char* atmosphereFragmentShader = R"(
#version 330 core
out vec4 FragColor;

in vec3 WorldPos;
in vec3 WorldNormal;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 atmosphereColor;
uniform float density;

void main()
{
    vec3 viewDir = normalize(viewPos - WorldPos);
    vec3 lightDir = normalize(lightPos - WorldPos);
    float mu = max(dot(WorldNormal, viewDir), 0.0);
    float horizon = pow(1.0 - mu, 2.7);
    float daylight = smoothstep(-0.28, 0.30, dot(WorldNormal, lightDir));
    float forward = pow(max(dot(viewDir, -lightDir), 0.0), 10.0);
    float alpha = horizon * daylight * density;
    vec3 color = atmosphereColor * (1.1 + forward * 0.8);
    FragColor = vec4(color * alpha * 1.7, alpha * 0.52);
}
)";

// ===== NEBULA PARTICLE SHADER =====
const char* nebulaVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in float aSize;
layout (location = 3) in float aDensity;
layout (location = 4) in float aRotation;

uniform mat4 view;
uniform mat4 projection;
uniform float time;
uniform float alphaScale;
uniform float opacityScale;

out vec3 particleColor;
out float particleDensity;
out float particleRotation;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = aSize / (gl_Position.w * 0.1);  // Distance-based sizing
    particleColor = aColor;
    particleDensity = aDensity * opacityScale;
    particleRotation = aRotation;
}
)";

const char* nebulaFragmentShader = R"(
#version 330 core
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
#version 330 core
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
#version 330 core
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
#version 330 core
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
    float phase = dot(aPos, vec3(0.017, 0.023, 0.011));
    // In vacuum stars do not atmospherically twinkle; retain only imperceptible
    // variation to avoid a digitally frozen background.
    float twinkle = 0.985 + 0.015 * sin(time * (0.25 + fract(phase) * 0.35) + phase);
    starColor *= twinkle * alphaScale;
}
)";

const char* enhancedStarFragmentShader = R"(
#version 330 core
out vec4 FragColor;

in vec3 starColor;
in float starSize;

void main()
{
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);

    // Sharp core with glowing halo
    float core = 1.0 - smoothstep(0.0, 0.13, dist);
    float halo = 1.0 - smoothstep(0.08, 0.5, dist);

    float alpha = core + halo * 0.4;

    // Brighter center
    vec3 color = starColor * (1.0 + core * 0.5);

    // Add subtle star flare effect for larger stars
    if (starSize > 2.5)
    {
        float flare = exp(-abs(coord.x) * 75.0) + exp(-abs(coord.y) * 75.0);
        alpha += flare * (1.0 - smoothstep(0.05, 0.5, dist)) * 0.18;
    }

    FragColor = vec4(color, alpha);
}
)";

// ===== HUD SHADER (2D OVERLAY) =====
const char* hudVertexShader = R"(
#version 330 core
layout (location = 0) in vec2 aPos;

uniform vec2 position;
uniform vec2 scale;

void main()
{
    gl_Position = vec4(aPos * scale + position, 0.0, 1.0);
}
)";

const char* hudFragmentShader = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 color;
uniform float alpha;

void main()
{
    FragColor = vec4(color, alpha);
}
)";

// ===== COORDINATE GRID SHADER =====
const char* gridVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 view;
uniform mat4 projection;

out vec3 gridColor;

void main()
{
    gl_Position = projection * view * vec4(aPos, 1.0);
    gridColor = aColor;
}
)";

const char* gridFragmentShader = R"(
#version 330 core
out vec4 FragColor;

in vec3 gridColor;

void main()
{
    FragColor = vec4(gridColor, 0.3);  // Semi-transparent grid
}
)";

// ===== PLANETARY RINGS SHADER =====
const char* ringVertexShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = vec3(worldPos);
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;

    gl_Position = projection * view * worldPos;
}
)";

const char* ringFragmentShader = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 ringColor;
uniform float ringAlpha;
uniform vec3 lightPos;

void main()
{
    float r = TexCoord.x;
    // Multi-scale optical-depth bands and a pronounced Cassini Division.
    float broadBands = 0.72 + 0.18 * sin(r * 78.0) + 0.10 * sin(r * 231.0 + 1.7);
    float fineBands = 0.90 + 0.10 * sin(r * 930.0);
    float cassini = 1.0 - 0.92 * exp(-pow((r - 0.61) * 75.0, 2.0));
    float encke = 1.0 - 0.55 * exp(-pow((r - 0.84) * 180.0, 2.0));
    float opticalDepth = clamp(broadBands * fineBands * cassini * encke, 0.04, 1.0);

    vec3 color = ringColor * (0.68 + opticalDepth * 0.42);

    // Add some lighting (top/bottom shading)
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = abs(dot(norm, lightDir)) * 0.72 + 0.16;

    color = color * diff;

    // Fade edges for realism
    float edgeFade = 1.0 - abs(TexCoord.x * 2.0 - 1.0);  // Fade at inner/outer edges
    edgeFade = smoothstep(0.0, 0.3, edgeFade);

    FragColor = vec4(color, ringAlpha * edgeFade * opticalDepth);
}
)";
