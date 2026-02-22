#pragma once
#include <string>
#include "Mat4.hpp"

class Shader
{
public:
    unsigned int ID;

    Shader(const char* vertexSrc, const char* fragmentSrc);

    void use() const;

    void setMat4(const std::string& name, const Mat4& matrix) const;
};