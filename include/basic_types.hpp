#pragma once

#include <cstdint>
#include <cstddef>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
using usize = std::size_t;

struct ioVec
{
    float x{};
    float y{};
    float z{};
    float padding{-1};
};

struct vec3
{
    float x;
    float y;
    float z;

    operator ioVec() const
    {
        return ioVec{x, y, z, 0};
    }
};