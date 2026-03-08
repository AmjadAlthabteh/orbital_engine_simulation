#pragma once
#include <string>
#include <map>
#include <vector>
#include <glad/glad.h>

// Texture loading and management system for planet textures
class TextureLoader
{
private:
    std::map<std::string, GLuint> textureCache;

public:
    TextureLoader();
    ~TextureLoader();

    // Load texture from file
    GLuint loadTexture(const std::string& filepath, bool generateMipmaps = true);

    // Load cubemap for skybox
    GLuint loadCubemap(const std::vector<std::string>& faces);

    // Create procedural texture for planets without image files
    GLuint createProceduralPlanetTexture(const std::string& planetName, int width = 512, int height = 512);

    // Get texture by name (from cache)
    GLuint getTexture(const std::string& name);

    // Clear all textures
    void clearAll();

private:
    // Generate specific procedural textures
    void generateEarthTexture(unsigned char* data, int width, int height);
    void generateMarsTexture(unsigned char* data, int width, int height);
    void generateJupiterTexture(unsigned char* data, int width, int height);
    void generateSaturnTexture(unsigned char* data, int width, int height);
    void generateMoonTexture(unsigned char* data, int width, int height);
    void generateSunTexture(unsigned char* data, int width, int height);
    void generateVenusTexture(unsigned char* data, int width, int height);
    void generateMercuryTexture(unsigned char* data, int width, int height);
    void generateUranusTexture(unsigned char* data, int width, int height);
    void generateNeptuneTexture(unsigned char* data, int width, int height);

    // Utility functions
    float noise2D(float x, float y, int seed = 0);
    float fbm(float x, float y, int octaves, int seed = 0);
};
