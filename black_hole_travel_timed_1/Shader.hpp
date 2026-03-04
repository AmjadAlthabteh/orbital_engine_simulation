#pragma once
#include <string>
#include "Mat4.hpp"
#include "Vec3.hpp"

class Shader
{
public:
    unsigned int ID;

    Shader(const char* vertexSrc, const char* fragmentSrc);

    void use() const;

    void setMat4(const std::string& name, const Mat4& matrix) const;
    void setVec3(const std::string& name, const Vec3& value) const;
    void setBool(const std::string& name, bool value) const;
};