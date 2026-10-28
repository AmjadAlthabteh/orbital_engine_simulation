#include "Renderer.hpp"
#include <glad/glad.h>

Renderer::Renderer(Shader* shader_)
    // Planet silhouettes need substantially more tessellation than debug markers;
    // 96x48 remains inexpensive for this small solar system and removes faceting.
    : shader(shader_), sphere(1.0f, 96, 48)
{
}

void Renderer::render(const Mat4& model,
    const Mat4& view,
    const Mat4& projection,
    const Vec3& color)
{
    shader->use();

    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setVec3("objectColor", color);

    sphere.draw();
}
