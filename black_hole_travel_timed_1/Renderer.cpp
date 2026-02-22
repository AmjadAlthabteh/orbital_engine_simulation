#include "Renderer.hpp"
#include <glad/glad.h>

Renderer::Renderer()
{
}

void Renderer::render(
    const std::vector<CelestialBody>& bodies,
    const Mat4& projection,
    const Mat4& view)
{
    for (const auto& body : bodies)
    {
        Mat4 model = Mat4::translate(body.position);
        Mat4 scale = Mat4::scale(body.radius);

        // Model = Translate * Scale (simplified)
        Mat4 finalModel = model;

        // Set uniforms through shader externally
        // Drawing sphere mesh would happen here
    }
}