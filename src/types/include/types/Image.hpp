#pragma once

#include <types/Basic.hpp>

#include <algorithm>
#include <vector>

namespace vrt::types
{
struct Image
{
    std::vector<u32>& pixels;
    u32 width;
    u32 height;

    u32 operator[](i32 x, i32 y) const
    {
        x = std::clamp(0, x, i32(width) - 1);
        y = std::clamp(0, y, i32(height) - 1);
        return pixels[x + y * width];
    }
};
}
