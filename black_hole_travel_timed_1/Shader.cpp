#include "Shader.hpp"
#include <glad/glad.h>
#include <iostream>

Shader::Shader(const char* vertexSrc, const char* fragmentSrc)
{
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSrc, nullptr);
    glCompileShader(vertex);

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSrc, nullptr);
    glCompileShader(fragment);

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() const
{
    glUseProgram(ID);
}

void Shader::setMat4(const std::string& name, const Mat4& matrix) const
{
    int loc = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, matrix.m);
}