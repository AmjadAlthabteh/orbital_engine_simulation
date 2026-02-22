#pragma once
#include "Shader.hpp"
#include "Sphere.hpp"
#include "Camera.hpp"

class Renderer
{
private:
    Shader* shader;
    Sphere sphere;

public:
    Renderer(Shader* shader);

    void render(const Mat4& model,
        const Mat4& view,
        const Mat4& projection);
};