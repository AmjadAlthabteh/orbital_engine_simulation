#pragma once
#include <vector>
#include "CelestialBody.hpp"
#include "Shader.hpp"

class Renderer
{
public:
    Renderer();

    void render(
        const std::vector<CelestialBody>& bodies,
        const Mat4& projection,
        const Mat4& view
    );
};