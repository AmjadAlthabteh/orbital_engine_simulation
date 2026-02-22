#pragma once
#include <cmath>

struct Mat4
{
    float m[16];

    Mat4()
    {
        for (int i = 0; i < 16; ++i)
            m[i] = 0.0f;
    }

    static Mat4 identity()
    {
        Mat4 r;
        r.m[0] = r.m[5] = r.m[10] = r.m[15] = 1.0f;
        return r;
    }

    static Mat4 translation(float x, float y, float z)
    {
        Mat4 r = identity();
        r.m[12] = x;
        r.m[13] = y;
        r.m[14] = z;
        return r;
    }

    static Mat4 scale(float s)
    {
        Mat4 r = identity();
        r.m[0] = r.m[5] = r.m[10] = s;
        return r;
    }

    static Mat4 perspective(float fov, float aspect, float near, float far)
    {
        Mat4 r;

        float tanHalf = tan(fov / 2.0f);

        r.m[0] = 1.0f / (aspect * tanHalf);
        r.m[5] = 1.0f / tanHalf;
        r.m[10] = -(far + near) / (far - near);
        r.m[11] = -1.0f;
        r.m[14] = -(2.0f * far * near) / (far - near);

        return r;
    }

    Mat4 operator*(const Mat4& o) const
    {
        Mat4 r;

        for (int col = 0; col < 4; ++col)
        {
            for (int row = 0; row < 4; ++row)
            {
                r.m[col * 4 + row] =
                    m[0 * 4 + row] * o.m[col * 4 + 0] +
                    m[1 * 4 + row] * o.m[col * 4 + 1] +
                    m[2 * 4 + row] * o.m[col * 4 + 2] +
                    m[3 * 4 + row] * o.m[col * 4 + 3];
            }
        }

        return r;
    }
};