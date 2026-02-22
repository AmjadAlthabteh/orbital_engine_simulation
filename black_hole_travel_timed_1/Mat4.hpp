#pragma once
#include <cmath>
#include "Vec3.hpp"

struct Mat4
{
    float m[16];

    static Mat4 identity()
    {
        Mat4 r{};
        r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.f;
        return r;
    }

    static Mat4 perspective(float fov, float aspect, float near, float far)
    {
        Mat4 r{};
        float tanHalf = std::tan(fov / 2.f);

        r.m[0] = 1.f / (aspect * tanHalf);
        r.m[5] = 1.f / tanHalf;
        r.m[10] = -(far + near) / (far - near);
        r.m[11] = -1.f;
        r.m[14] = -(2.f * far * near) / (far - near);

        return r;
    }

    static Mat4 translate(const Vec3& v)
    {
        Mat4 r = identity();
        r.m[12] = v.x;
        r.m[13] = v.y;
        r.m[14] = v.z;
        return r;
    }

    static Mat4 scale(float s)
    {
        Mat4 r = identity();
        r.m[0] = r.m[5] = r.m[10] = s;
        return r;
    }
};