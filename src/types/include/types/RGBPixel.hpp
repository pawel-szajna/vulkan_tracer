#pragma once

#include <types/Basic.hpp>

namespace vrt::types
{
struct RGBPixel
{
    u32 r;
    u32 g;
    u32 b;

    RGBPixel(u32 compressed) // NOLINT(google-explicit-constructor)
            : r{(compressed & 0xff000000) >> 24}
            , g{(compressed & 0x00ff0000) >> 16}
            , b{(compressed & 0x0000ff00) >> 8}
    {}

    RGBPixel(u32 r, u32 g, u32 b)
            : r{r}, g{g}, b{b}
    {}

    RGBPixel operator*(u32 val) const
    {
        return {r * val, g * val, b * val};
    }

    RGBPixel operator/(u32 val) const
    {
        return {r / val, g / val, b / val};
    }

    RGBPixel operator+(RGBPixel val) const
    {
        return {r + val.r, g + val.g, b + val.b};
    }

    explicit operator u32() const
    {
        return (r << 24) | (g << 16) | (b << 8) | 0xff;
    }
};
}