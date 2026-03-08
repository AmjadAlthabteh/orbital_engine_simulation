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

            // Use noise to create continents and oceans
            float nx = x / (float)width;
            float ny = y / (float)height;

            float landValue = fbm(nx * 8.0f, ny * 8.0f, 4, 12345);
            float cloudValue = fbm(nx * 12.0f, ny * 12.0f, 3, 54321);

            // Ocean vs land threshold
            if (landValue > 0.5f)
            {
                // Land (green/brown)
                float greenValue = (landValue - 0.5f) * 2.0f;
                data[idx + 0] = static_cast<unsigned char>(34 + greenValue * 80);  // R
                data[idx + 1] = static_cast<unsigned char>(139 + greenValue * 50);  // G
                data[idx + 2] = static_cast<unsigned char>(34);  // B
            }
            else
            {
                // Ocean (blue)
                float oceanDepth = landValue * 2.0f;
                data[idx + 0] = static_cast<unsigned char>(10 + oceanDepth * 40);  // R
                data[idx + 1] = static_cast<unsigned char>(50 + oceanDepth * 60);  // G
                data[idx + 2] = static_cast<unsigned char>(150 + oceanDepth * 80);  // B
            }

            // Add clouds
            if (cloudValue > 0.7f)
            {
                float cloudIntensity = (cloudValue - 0.7f) * 3.33f;
                data[idx + 0] = static_cast<unsigned char>(std::min(255.0f, data[idx + 0] + cloudIntensity * 100));
                data[idx + 1] = static_cast<unsigned char>(std::min(255.0f, data[idx + 1] + cloudIntensity * 100));
                data[idx + 2] = static_cast<unsigned char>(std::min(255.0f, data[idx + 2] + cloudIntensity * 100));
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

            float terrainValue = fbm(nx * 10.0f, ny * 10.0f, 5, 99999);

            // Mars red/orange colors
            unsigned char baseR = 200 + static_cast<unsigned char>(terrainValue * 55);
            unsigned char baseG = 80 + static_cast<unsigned char>(terrainValue * 50);
            unsigned char baseB = 30 + static_cast<unsigned char>(terrainValue * 30);

            data[idx + 0] = baseR;
            data[idx + 1] = baseG;
            data[idx + 2] = baseB;

            // Add dark craters
            float craterNoise = noise2D(nx * 30.0f, ny * 30.0f, 77777);
            if (craterNoise > 0.85f)
            {
                float darkness = (craterNoise - 0.85f) * 6.67f;
                data[idx + 0] = static_cast<unsigned char>(data[idx + 0] * (1.0f - darkness * 0.5f));
                data[idx + 1] = static_cast<unsigned char>(data[idx + 1] * (1.0f - darkness * 0.5f));
                data[idx + 2] = static_cast<unsigned char>(data[idx + 2] * (1.0f - darkness * 0.5f));
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

            float craterValue = fbm(nx * 12.0f, ny * 12.0f, 4, 88888);

            // Gray surface
            unsigned char gray = 80 + static_cast<unsigned char>(craterValue * 100);

            data[idx + 0] = gray;
            data[idx + 1] = gray;
            data[idx + 2] = gray;
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
