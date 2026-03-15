#pragma once
#include <glad/glad.h>
#include "Shader.hpp"
#include "Framebuffer.hpp"
#include <memory>

// Post-processing shaders for Bloom and HDR
const char* bloomExtractVertexShader = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char* bloomExtractFragmentShader = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform float threshold;

void main()
{
    vec3 color = texture(sceneTexture, TexCoords).rgb;

    // Extract bright areas (above threshold)
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > threshold)
        FragColor = vec4(color, 1.0);
    else
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
)";

const char* blurVertexShader = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char* blurFragmentShader = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;
uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
    vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
    vec3 result = texture(image, TexCoords).rgb * weight[0]; // current fragment's contribution

    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
)";

const char* hdrVertexShader = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char* hdrFragmentShader = R"(
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform sampler2D bloomTexture;
uniform float exposure;
uniform float bloomStrength;

void main()
{
    vec3 hdrColor = texture(sceneTexture, TexCoords).rgb;
    vec3 bloomColor = texture(bloomTexture, TexCoords).rgb;

    // Add bloom to scene
    hdrColor += bloomColor * bloomStrength;

    // Tone mapping (Reinhard)
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);

    // Gamma correction
    mapped = pow(mapped, vec3(1.0 / 2.2));

    FragColor = vec4(mapped, 1.0);
}
)";

class PostProcessing
{
private:
    // Shaders
    std::unique_ptr<Shader> bloomExtractShader;
    std::unique_ptr<Shader> blurShader;
    std::unique_ptr<Shader> hdrShader;

    // Framebuffers for ping-pong blur
    std::unique_ptr<Framebuffer> pingpongFBO[2];

    // Screen quad VAO
    GLuint quadVAO, quadVBO;

    int width, height;

    // Parameters
    float bloomThreshold;
    float bloomStrength;
    float exposure;

public:
    PostProcessing(int w, int h)
        : width(w), height(h)
        , bloomThreshold(1.0f)
        , bloomStrength(0.04f)
        , exposure(1.0f)
    {
        // Create shaders
        bloomExtractShader = std::make_unique<Shader>(bloomExtractVertexShader, bloomExtractFragmentShader);
        blurShader = std::make_unique<Shader>(blurVertexShader, blurFragmentShader);
        hdrShader = std::make_unique<Shader>(hdrVertexShader, hdrFragmentShader);

        // Create ping-pong framebuffers for blur
        pingpongFBO[0] = std::make_unique<Framebuffer>(width, height);
        pingpongFBO[1] = std::make_unique<Framebuffer>(width, height);

        // Setup screen quad
        setupQuad();
    }

    void setupQuad()
    {
        float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    void process(GLuint sceneTexture)
    {
        glDisable(GL_DEPTH_TEST);

        // 1. Extract bright areas
        pingpongFBO[0]->bind();
        glClear(GL_COLOR_BUFFER_BIT);
        bloomExtractShader->use();
        bloomExtractShader->setFloat("threshold", bloomThreshold);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneTexture);
        renderQuad();

        // 2. Blur bright areas (ping-pong)
        bool horizontal = true;
        int blurIterations = 10;

        for (int i = 0; i < blurIterations; i++)
        {
            pingpongFBO[horizontal]->bind();
            blurShader->use();
            blurShader->setBool("horizontal", horizontal);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pingpongFBO[!horizontal]->getColorTexture());
            renderQuad();
            horizontal = !horizontal;
        }

        // 3. Combine scene with bloom (render to default framebuffer)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        hdrShader->use();
        hdrShader->setFloat("exposure", exposure);
        hdrShader->setFloat("bloomStrength", bloomStrength);
        hdrShader->setInt("sceneTexture", 0);
        hdrShader->setInt("bloomTexture", 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, pingpongFBO[!horizontal]->getColorTexture());

        renderQuad();

        glEnable(GL_DEPTH_TEST);
    }

    void renderQuad()
    {
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void resize(int w, int h)
    {
        width = w;
        height = h;
        pingpongFBO[0]->resize(w, h);
        pingpongFBO[1]->resize(w, h);
    }

    // Setters for parameters
    void setBloomThreshold(float threshold) { bloomThreshold = threshold; }
    void setBloomStrength(float strength) { bloomStrength = strength; }
    void setExposure(float exp) { exposure = exp; }

    // Getters
    float getBloomThreshold() const { return bloomThreshold; }
    float getBloomStrength() const { return bloomStrength; }
    float getExposure() const { return exposure; }

    ~PostProcessing()
    {
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
    }
};
