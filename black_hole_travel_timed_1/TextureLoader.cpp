#include "TextureLoader.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

// Note: For production use, integrate stb_image.h for loading actual image files
// For now, we'll use procedural generation

TextureLoader::TextureLoader()
{
}

TextureLoader::~TextureLoader()
{
    clearAll();
}

GLuint TextureLoader::loadTexture(const std::string& filepath, bool generateMipmaps)
{
    // Check cache first
    auto it = textureCache.find(filepath);
    if (it != textureCache.end())
    {
        return it->second;
    }

    // For now, return 0 (no texture) since we're using procedural generation
    // In production, use stb_image to load from file:
    /*
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        // Create OpenGL texture...
    }
    */

    return 0;
}

GLuint TextureLoader::loadCubemap(const std::vector<std::string>& faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    // Would load cubemap faces here using stb_image
    // For now, return placeholder
    return textureID;
}

GLuint TextureLoader::createProceduralPlanetTexture(const std::string& planetName, int width, int height)
{
    // Check cache
    auto it = textureCache.find(planetName);
    if (it != textureCache.end())
    {
        return it->second;
    }

    // Allocate texture data (RGB)
    std::vector<unsigned char> data(width * height * 3);

    // Generate based on planet name
    if (planetName == "Earth")
        generateEarthTexture(data.data(), width, height);
    else if (planetName == "Mars")
        generateMarsTexture(data.data(), width, height);
    else if (planetName == "Jupiter")
        generateJupiterTexture(data.data(), width, height);
    else if (planetName == "Saturn")
        generateSaturnTexture(data.data(), width, height);
    else if (planetName == "Sun")
        generateSunTexture(data.data(), width, height);
    else if (planetName == "Venus")
        generateVenusTexture(data.data(), width, height);
    else if (planetName == "Mercury")
        generateMercuryTexture(data.data(), width, height);
    else if (planetName == "Uranus")
        generateUranusTexture(data.data(), width, height);
    else if (planetName == "Neptune")
        generateNeptuneTexture(data.data(), width, height);
    else
        generateMoonTexture(data.data(), width, height);  // Default

    // Create OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    // Cache it
    textureCache[planetName] = textureID;

    std::cout << "Generated procedural texture for " << planetName << " (" << width << "x" << height << ")\n";

    return textureID;
}

GLuint TextureLoader::getTexture(const std::string& name)
{
    auto it = textureCache.find(name);
    if (it != textureCache.end())
    {
        return it->second;
    }
    return 0;
}

void TextureLoader::clearAll()
{
    for (auto& pair : textureCache)
    {
        glDeleteTextures(1, &pair.second);
    }
    textureCache.clear();
}

// Procedural texture generators
void TextureLoader::generateEarthTexture(unsigned char* data, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = (y * width + x) * 3;

            // Normalized coordinates [0, 1]
            float nx = x / (float)width;
            float ny = y / (float)height;

            // Latitude-based calculations (for polar caps)
            float latitude = (ny - 0.5f) * 2.0f;  // -1 at bottom, +1 at top
            float absLatitude = std::abs(latitude);

            // Multi-scale terrain noise for realistic continents
            float continentNoise = fbm(nx * 4.0f, ny * 4.0f, 5, 12345);    // Large landmasses
            float detailNoise = fbm(nx * 16.0f, ny * 16.0f, 4, 23456);     // Coastline detail
            float mountainNoise = fbm(nx * 24.0f, ny * 24.0f, 6, 34567);   // Mountain ranges

            // Combine for realistic terrain (0 = deep ocean, 1 = high mountain)
            float terrain = continentNoise * 0.6f + detailNoise * 0.3f + mountainNoise * 0.1f;

            // Polar ice caps (latitude-based)
            bool isPolar = absLatitude > 0.85f;
            float polarIntensity = (absLatitude - 0.85f) / 0.15f;  // 0 to 1 in polar regions
            polarIntensity = std::max(0.0f, std::min(1.0f, polarIntensity));

            // Terrain classification
            if (isPolar && polarIntensity > 0.3f)
            {
                // Polar ice caps (bright white with slight blue tint)
                unsigned char iceValue = 220 + static_cast<unsigned char>(polarIntensity * 35);
                data[idx + 0] = iceValue - 10;  // Slight blue tint
                data[idx + 1] = iceValue - 5;
                data[idx + 2] = iceValue;
            }
            else if (terrain > 0.52f)  // Land threshold
            {
                float landHeight = (terrain - 0.52f) / 0.48f;  // Normalize to [0, 1]

                if (landHeight > 0.7f)
                {
                    // High mountains (brown/gray peaks with snow)
                    float mountainHeight = (landHeight - 0.7f) / 0.3f;
                    unsigned char baseGray = 100 + static_cast<unsigned char>(mountainHeight * 100);
                    data[idx + 0] = baseGray + 20;  // Brownish
                    data[idx + 1] = baseGray + 10;
                    data[idx + 2] = baseGray;

                    // Snow on peaks
                    if (mountainHeight > 0.6f)
                    {
                        float snowAmount = (mountainHeight - 0.6f) / 0.4f;
                        data[idx + 0] = static_cast<unsigned char>(data[idx + 0] + snowAmount * 100);
                        data[idx + 1] = static_cast<unsigned char>(data[idx + 1] + snowAmount * 110);
                        data[idx + 2] = static_cast<unsigned char>(data[idx + 2] + snowAmount * 120);
                    }
                }
                else if (landHeight > 0.35f)
                {
                    // Mid-elevation land (green vegetation)
                    float greenValue = (landHeight - 0.35f) / 0.35f;
                    data[idx + 0] = 34 + static_cast<unsigned char>(greenValue * 50);   // R
                    data[idx + 1] = 139 + static_cast<unsigned char>(greenValue * 70);  // G (vibrant green)
                    data[idx + 2] = 34 + static_cast<unsigned char>(greenValue * 20);   // B
                }
                else
                {
                    // Low elevation land (sandy/desert)
                    float sandValue = landHeight / 0.35f;
                    data[idx + 0] = 194 + static_cast<unsigned char>(sandValue * 50);
                    data[idx + 1] = 178 + static_cast<unsigned char>(sandValue * 40);
                    data[idx + 2] = 128 + static_cast<unsigned char>(sandValue * 30);
                }
            }
            else
            {
                // Ocean (varied depth)
                float oceanDepth = terrain / 0.52f;  // 0 = deepest, 1 = shallow coast

                if (oceanDepth > 0.85f)
                {
                    // Shallow coastal water (cyan/turquoise)
                    data[idx + 0] = 64 + static_cast<unsigned char>((oceanDepth - 0.85f) * 200);
                    data[idx + 1] = 164 + static_cast<unsigned char>((oceanDepth - 0.85f) * 200);
                    data[idx + 2] = 223;
                }
                else if (oceanDepth > 0.5f)
                {
                    // Medium depth (standard blue)
                    data[idx + 0] = 10 + static_cast<unsigned char>(oceanDepth * 50);
                    data[idx + 1] = 50 + static_cast<unsigned char>(oceanDepth * 100);
                    data[idx + 2] = 150 + static_cast<unsigned char>(oceanDepth * 80);
                }
                else
                {
                    // Deep ocean (dark blue/navy)
                    data[idx + 0] = 0 + static_cast<unsigned char>(oceanDepth * 30);
                    data[idx + 1] = 20 + static_cast<unsigned char>(oceanDepth * 60);
                    data[idx + 2] = 80 + static_cast<unsigned char>(oceanDepth * 100);
                }
            }

            // Add wispy clouds (scattered, not everywhere)
            float cloudValue = fbm(nx * 12.0f, ny * 12.0f, 3, 54321);
            if (cloudValue > 0.75f && !isPolar)  // No clouds over ice caps
            {
                float cloudIntensity = (cloudValue - 0.75f) * 4.0f;  // 0 to 1
                cloudIntensity = std::min(1.0f, cloudIntensity);

                // Blend with white clouds
                data[idx + 0] = static_cast<unsigned char>(data[idx + 0] * (1.0f - cloudIntensity * 0.8f) + 255 * cloudIntensity * 0.8f);
                data[idx + 1] = static_cast<unsigned char>(data[idx + 1] * (1.0f - cloudIntensity * 0.8f) + 255 * cloudIntensity * 0.8f);
                data[idx + 2] = static_cast<unsigned char>(data[idx + 2] * (1.0f - cloudIntensity * 0.8f) + 255 * cloudIntensity * 0.8f);
            }
        }
    }
}

void TextureLoader::generateMarsTexture(unsigned char* data, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = (y * width + x) * 3;

            float nx = x / (float)width;
            float ny = y / (float)height;

            // Latitude for polar caps and Valles Marineris (equatorial canyon)
            float latitude = (ny - 0.5f) * 2.0f;  // -1 to +1
            float absLatitude = std::abs(latitude);

            // Multi-scale terrain variation
            float terrainBase = fbm(nx * 8.0f, ny * 8.0f, 5, 99999);      // Large features
            float terrainDetail = fbm(nx * 20.0f, ny * 20.0f, 4, 88888);  // Medium detail
            float dustNoise = fbm(nx * 35.0f, ny * 35.0f, 3, 77777);      // Fine dust patterns

            // Combine terrain layers
            float terrain = terrainBase * 0.5f + terrainDetail * 0.3f + dustNoise * 0.2f;

            // Polar ice caps (CO2 and water ice)
            bool isPolar = absLatitude > 0.82f;
            float polarIntensity = (absLatitude - 0.82f) / 0.18f;
            polarIntensity = std::max(0.0f, std::min(1.0f, polarIntensity));

            // Valles Marineris (massive equatorial canyon system)
            float canyonDist = std::abs(latitude);  // Distance from equator
            bool isCanyon = (canyonDist < 0.15f) && (nx > 0.3f && nx < 0.7f);  // Horizontal band
            float canyonDepth = 0.0f;
            if (isCanyon)
            {
                float canyonNoise = fbm(nx * 40.0f, ny * 10.0f, 3, 55555);
                canyonDepth = (0.15f - canyonDist) / 0.15f * canyonNoise;
                canyonDepth = std::max(0.0f, std::min(1.0f, canyonDepth));
            }

            if (isPolar && polarIntensity > 0.2f)
            {
                // Polar ice caps (bright white/pinkish ice)
                unsigned char iceValue = 200 + static_cast<unsigned char>(polarIntensity * 55);
                data[idx + 0] = iceValue;         // Bright
                data[idx + 1] = iceValue - 20;    // Less green
                data[idx + 2] = iceValue - 30;    // Pinkish tint
            }
            else if (isCanyon && canyonDepth > 0.3f)
            {
                // Valles Marineris canyon (very dark, shadowed)
                float shadowDarkness = canyonDepth * 0.7f;
                data[idx + 0] = static_cast<unsigned char>(80 * (1.0f - shadowDarkness));
                data[idx + 1] = static_cast<unsigned char>(30 * (1.0f - shadowDarkness));
                data[idx + 2] = static_cast<unsigned char>(20 * (1.0f - shadowDarkness));
            }
            else
            {
                // Standard Martian terrain (rusty red/orange)
                float elevation = terrain;

                if (elevation > 0.65f)
                {
                    // High terrain (bright orange/tan - like Olympus Mons)
                    unsigned char brightR = 220 + static_cast<unsigned char>((elevation - 0.65f) * 100);
                    unsigned char brightG = 140 + static_cast<unsigned char>((elevation - 0.65f) * 80);
                    unsigned char brightB = 70 + static_cast<unsigned char>((elevation - 0.65f) * 50);
                    data[idx + 0] = std::min(255, (int)brightR);
                    data[idx + 1] = std::min(255, (int)brightG);
                    data[idx + 2] = std::min(255, (int)brightB);
                }
                else if (elevation > 0.35f)
                {
                    // Mid-elevation (standard rusty red)
                    data[idx + 0] = 193 + static_cast<unsigned char>(elevation * 50);
                    data[idx + 1] = 68 + static_cast<unsigned char>(elevation * 60);
                    data[idx + 2] = 14 + static_cast<unsigned char>(elevation * 40);
                }
                else
                {
                    // Low terrain (darker, ancient lava plains)
                    data[idx + 0] = 150 + static_cast<unsigned char>(elevation * 80);
                    data[idx + 1] = 50 + static_cast<unsigned char>(elevation * 50);
                    data[idx + 2] = 20 + static_cast<unsigned char>(elevation * 30);
                }
            }

            // Heavy cratering (Mars is heavily cratered)
            float craterNoise1 = noise2D(nx * 50.0f, ny * 50.0f, 77777);
            float craterNoise2 = noise2D(nx * 80.0f, ny * 80.0f, 66666);

            // Large craters
            if (craterNoise1 > 0.88f && !isPolar)
            {
                float craterIntensity = (craterNoise1 - 0.88f) / 0.12f;
                float darkness = craterIntensity * 0.6f;
                data[idx + 0] = static_cast<unsigned char>(data[idx + 0] * (1.0f - darkness));
                data[idx + 1] = static_cast<unsigned char>(data[idx + 1] * (1.0f - darkness));
                data[idx + 2] = static_cast<unsigned char>(data[idx + 2] * (1.0f - darkness));
            }

            // Small craters (more frequent)
            if (craterNoise2 > 0.92f && !isPolar)
            {
                float smallCraterIntensity = (craterNoise2 - 0.92f) / 0.08f;
                float darkness = smallCraterIntensity * 0.4f;
                data[idx + 0] = static_cast<unsigned char>(data[idx + 0] * (1.0f - darkness));
                data[idx + 1] = static_cast<unsigned char>(data[idx + 1] * (1.0f - darkness));
                data[idx + 2] = static_cast<unsigned char>(data[idx + 2] * (1.0f - darkness));
            }

            // Dust storms (occasional bright spots)
            float dustStorm = fbm(nx * 15.0f, ny * 15.0f, 2, 44444);
            if (dustStorm > 0.85f && !isPolar)
            {
                float stormIntensity = (dustStorm - 0.85f) / 0.15f;
                data[idx + 0] = static_cast<unsigned char>(std::min(255.0f, data[idx + 0] + stormIntensity * 60));
                data[idx + 1] = static_cast<unsigned char>(std::min(255.0f, data[idx + 1] + stormIntensity * 40));
                data[idx + 2] = static_cast<unsigned char>(std::min(255.0f, data[idx + 2] + stormIntensity * 20));
            }
        }
    }
}

void TextureLoader::generateJupiterTexture(unsigned char* data, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = (y * width + x) * 3;

            float nx = x / (float)width;
            float ny = y / (float)height;

            // Horizontal bands
            float bandValue = std::sin(ny * 20.0f + fbm(nx * 5.0f, ny * 2.0f, 3, 11111) * 2.0f);

            // Jupiter colors (orange, white, brown bands)
            if (bandValue > 0.3f)
            {
                // Light bands
                data[idx + 0] = 230 + static_cast<unsigned char>((bandValue - 0.3f) * 25);
                data[idx + 1] = 200 + static_cast<unsigned char>((bandValue - 0.3f) * 35);
                data[idx + 2] = 150 + static_cast<unsigned char>((bandValue - 0.3f) * 50);
            }
            else
            {
                // Dark bands (brownish)
                data[idx + 0] = 180 + static_cast<unsigned char>(bandValue * 50);
                data[idx + 1] = 130 + static_cast<unsigned char>(bandValue * 50);
                data[idx + 2] = 80 + static_cast<unsigned char>(bandValue * 40);
            }

            // Great Red Spot
            float spotX = 0.3f;
            float spotY = 0.6f;
            float dx = nx - spotX;
            float dy = (ny - spotY) * 2.0f;  // Elliptical
            float dist = std::sqrt(dx * dx + dy * dy);

            if (dist < 0.1f)
            {
                float spotIntensity = (0.1f - dist) * 10.0f;
                data[idx + 0] = static_cast<unsigned char>(std::min(255.0f, data[idx + 0] + spotIntensity * 75));
                data[idx + 1] = static_cast<unsigned char>(data[idx + 1] * (1.0f - spotIntensity * 0.3f));
                data[idx + 2] = static_cast<unsigned char>(data[idx + 2] * (1.0f - spotIntensity * 0.5f));
            }
        }
    }
}

void TextureLoader::generateSaturnTexture(unsigned char* data, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = (y * width + x) * 3;

            float nx = x / (float)width;
            float ny = y / (float)height;

            // Subtle horizontal bands (less pronounced than Jupiter)
            float bandValue = std::sin(ny * 15.0f + fbm(nx * 4.0f, ny * 1.5f, 2, 22222) * 1.5f);

            // Saturn golden/tan colors
            unsigned char baseR = 230 + static_cast<unsigned char>(bandValue * 25);
            unsigned char baseG = 210 + static_cast<unsigned char>(bandValue * 30);
            unsigned char baseB = 150 + static_cast<unsigned char>(bandValue * 50);

            data[idx + 0] = baseR;
            data[idx + 1] = baseG;
            data[idx + 2] = baseB;
        }
    }
}

void TextureLoader::generateSunTexture(unsigned char* data, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = (y * width + x) * 3;

            float nx = x / (float)width;
            float ny = y / (float)height;

            // Plasma-like surface
            float plasma = fbm(nx * 6.0f, ny * 6.0f, 4, 33333);

            // Bright yellow/orange
            data[idx + 0] = 255;  // Full red
            data[idx + 1] = static_cast<unsigned char>(200 + plasma * 55);  // Orange-yellow
            data[idx + 2] = static_cast<unsigned char>(50 + plasma * 100);  // Some yellow
        }
    }
}

void TextureLoader::generateVenusTexture(unsigned char* data, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = (y * width + x) * 3;

            float nx = x / (float)width;
            float ny = y / (float)height;

            // Thick cloud cover
            float cloudValue = fbm(nx * 8.0f, ny * 8.0f, 4, 44444);

            // Yellowish clouds
            unsigned char baseR = 240 + static_cast<unsigned char>(cloudValue * 15);
            unsigned char baseG = 220 + static_cast<unsigned char>(cloudValue * 35);
            unsigned char baseB = 130 + static_cast<unsigned char>(cloudValue * 70);

            data[idx + 0] = baseR;
            data[idx + 1] = baseG;
            data[idx + 2] = baseB;
        }
    }
}

void TextureLoader::generateMercuryTexture(unsigned char* data, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = (y * width + x) * 3;

            float nx = x / (float)width;
            float ny = y / (float)height;

            float craterDensity = fbm(nx * 15.0f, ny * 15.0f, 5, 55555);

            // Gray rocky surface
            unsigned char gray = 100 + static_cast<unsigned char>(craterDensity * 80);

            data[idx + 0] = gray + 20;  // Slightly brownish
            data[idx + 1] = gray;
            data[idx + 2] = gray - 10;
        }
    }
}

void TextureLoader::generateUranusTexture(unsigned char* data, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = (y * width + x) * 3;

            float nx = x / (float)width;
            float ny = y / (float)height;

            float cloudValue = fbm(nx * 5.0f, ny * 5.0f, 3, 66666);

            // Cyan/light blue
            unsigned char baseR = 100 + static_cast<unsigned char>(cloudValue * 60);
            unsigned char baseG = 200 + static_cast<unsigned char>(cloudValue * 55);
            unsigned char baseB = 220 + static_cast<unsigned char>(cloudValue * 35);

            data[idx + 0] = baseR;
            data[idx + 1] = baseG;
            data[idx + 2] = baseB;
        }
    }
}

void TextureLoader::generateNeptuneTexture(unsigned char* data, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = (y * width + x) * 3;

            float nx = x / (float)width;
            float ny = y / (float)height;

            float cloudValue = fbm(nx * 6.0f, ny * 6.0f, 3, 77777);

            // Deep blue
            unsigned char baseR = 30 + static_cast<unsigned char>(cloudValue * 50);
            unsigned char baseG = 60 + static_cast<unsigned char>(cloudValue * 70);
            unsigned char baseB = 180 + static_cast<unsigned char>(cloudValue * 75);

            data[idx + 0] = baseR;
            data[idx + 1] = baseG;
            data[idx + 2] = baseB;
        }
    }
}

void TextureLoader::generateMoonTexture(unsigned char* data, int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = (y * width + x) * 3;

            float nx = x / (float)width;
            float ny = y / (float)height;

            // Base terrain (highlands vs maria)
            float baseNoise = fbm(nx * 6.0f, ny * 6.0f, 5, 88888);       // Large features
            float detailNoise = fbm(nx * 20.0f, ny * 20.0f, 4, 77777);   // Fine detail

            // Combine for terrain classification
            float terrain = baseNoise * 0.7f + detailNoise * 0.3f;

            // Determine if this is maria (dark lava plains) or highlands (bright)
            bool isMaria = terrain < 0.42f;  // About 30% of surface is maria

            unsigned char baseGray;
            if (isMaria)
            {
                // Maria (dark basaltic plains - like Mare Tranquillitatis)
                baseGray = 60 + static_cast<unsigned char>(terrain * 60);
            }
            else
            {
                // Highlands (bright, heavily cratered ancient crust)
                float heightValue = (terrain - 0.42f) / 0.58f;
                baseGray = 140 + static_cast<unsigned char>(heightValue * 80);
            }

            data[idx + 0] = baseGray;
            data[idx + 1] = baseGray;
            data[idx + 2] = baseGray;

            // Heavy cratering (Moon is heavily cratered due to no atmosphere)
            // Large craters (like Tycho, Copernicus)
            float largeCrater = noise2D(nx * 25.0f, ny * 25.0f, 11111);
            if (largeCrater > 0.85f)
            {
                float craterIntensity = (largeCrater - 0.85f) / 0.15f;

                // Crater center (dark)
                if (largeCrater > 0.92f)
                {
                    float darkness = (largeCrater - 0.92f) / 0.08f;
                    data[idx + 0] = static_cast<unsigned char>(data[idx + 0] * (1.0f - darkness * 0.7f));
                    data[idx + 1] = static_cast<unsigned char>(data[idx + 1] * (1.0f - darkness * 0.7f));
                    data[idx + 2] = static_cast<unsigned char>(data[idx + 2] * (1.0f - darkness * 0.7f));
                }
                else
                {
                    // Crater rim (bright ejecta)
                    float brightness = craterIntensity * 0.4f;
                    data[idx + 0] = static_cast<unsigned char>(std::min(255.0f, data[idx + 0] + brightness * 80));
                    data[idx + 1] = static_cast<unsigned char>(std::min(255.0f, data[idx + 1] + brightness * 80));
                    data[idx + 2] = static_cast<unsigned char>(std::min(255.0f, data[idx + 2] + brightness * 80));
                }
            }

            // Medium craters (more frequent)
            float mediumCrater = noise2D(nx * 45.0f, ny * 45.0f, 22222);
            if (mediumCrater > 0.88f)
            {
                float craterDepth = (mediumCrater - 0.88f) / 0.12f;
                float darkness = craterDepth * 0.5f;
                data[idx + 0] = static_cast<unsigned char>(data[idx + 0] * (1.0f - darkness));
                data[idx + 1] = static_cast<unsigned char>(data[idx + 1] * (1.0f - darkness));
                data[idx + 2] = static_cast<unsigned char>(data[idx + 2] * (1.0f - darkness));
            }

            // Small craters (very frequent - micrometeorite impacts)
            float smallCrater = noise2D(nx * 70.0f, ny * 70.0f, 33333);
            if (smallCrater > 0.92f)
            {
                float tiny = (smallCrater - 0.92f) / 0.08f;
                float darkness = tiny * 0.3f;
                data[idx + 0] = static_cast<unsigned char>(data[idx + 0] * (1.0f - darkness));
                data[idx + 1] = static_cast<unsigned char>(data[idx + 1] * (1.0f - darkness));
                data[idx + 2] = static_cast<unsigned char>(data[idx + 2] * (1.0f - darkness));
            }

            // Rays from fresh craters (bright streaks radiating outward)
            float rayNoise = fbm(nx * 30.0f, ny * 30.0f, 2, 44444);
            if (rayNoise > 0.88f)
            {
                float rayIntensity = (rayNoise - 0.88f) / 0.12f;
                float brightness = rayIntensity * 0.3f;
                data[idx + 0] = static_cast<unsigned char>(std::min(255.0f, data[idx + 0] + brightness * 60));
                data[idx + 1] = static_cast<unsigned char>(std::min(255.0f, data[idx + 1] + brightness * 60));
                data[idx + 2] = static_cast<unsigned char>(std::min(255.0f, data[idx + 2] + brightness * 60));
            }

            // Rilles (sinuous channels in maria)
            if (isMaria)
            {
                float rilleNoise = fbm(nx * 40.0f, ny * 15.0f, 2, 55555);
                if (rilleNoise > 0.82f && rilleNoise < 0.85f)
                {
                    // Dark channel
                    data[idx + 0] = static_cast<unsigned char>(data[idx + 0] * 0.7f);
                    data[idx + 1] = static_cast<unsigned char>(data[idx + 1] * 0.7f);
                    data[idx + 2] = static_cast<unsigned char>(data[idx + 2] * 0.7f);
                }
            }
        }
    }
}

// Simple noise implementation
float TextureLoader::noise2D(float x, float y, int seed)
{
    // Simple hash-based noise
    int xi = static_cast<int>(std::floor(x));
    int yi = static_cast<int>(std::floor(y));

    float xf = x - xi;
    float yf = y - yi;

    // Hash function
    auto hash = [seed](int x, int y) -> float {
        int n = x + y * 57 + seed * 131;
        n = (n << 13) ^ n;
        return (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
    };

    // Get corner values
    float v00 = hash(xi, yi);
    float v10 = hash(xi + 1, yi);
    float v01 = hash(xi, yi + 1);
    float v11 = hash(xi + 1, yi + 1);

    // Smooth interpolation
    auto smoothstep = [](float t) -> float {
        return t * t * (3.0f - 2.0f * t);
    };

    float sx = smoothstep(xf);
    float sy = smoothstep(yf);

    // Bilinear interpolation
    float v0 = v00 * (1.0f - sx) + v10 * sx;
    float v1 = v01 * (1.0f - sx) + v11 * sx;

    return (v0 * (1.0f - sy) + v1 * sy + 1.0f) * 0.5f;  // Map to [0, 1]
}

float TextureLoader::fbm(float x, float y, int octaves, int seed)
{
    float value = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float maxValue = 0.0f;

    for (int i = 0; i < octaves; i++)
    {
        value += noise2D(x * frequency, y * frequency, seed + i) * amplitude;
        maxValue += amplitude;
        amplitude *= 0.5f;
        frequency *= 2.0f;
    }

    return value / maxValue;
}
