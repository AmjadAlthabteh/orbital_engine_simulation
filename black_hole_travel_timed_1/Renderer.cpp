#include "Renderer.hpp"
#include <glad/glad.h>

Renderer::Renderer(Shader* shader_)
    : shader(shader_), sphere(1.0f)
{
}

void Renderer::render(const Mat4& model,
    const Mat4& view,
    const Mat4& projection)
{
    shader->use();

    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    sphere.draw();
}