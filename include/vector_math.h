#pragma once
#include <utils.h>

struct v2
{
    f32 x;
    f32 y;
};

f32 v2_dot(const v2 x, const v2 y)
{
    return (x.x * y.x) + (x.y * y.y);
}

// TODO handle 0 length vector
v2 v2_normalize(const v2 x)
{
    auto bottom = sqrtf(x.x * x.x + x.y * x.y);
    return {x.x / bottom, x.y / bottom};
}

v2 v2_reflect(const v2 x, const v2 normal)
{
    f32 dot = 2.0f * v2_dot(x, normal);

    return {x.x - dot * normal.x, x.y - dot * normal.y};
}