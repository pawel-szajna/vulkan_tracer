#pragma once

#include <cstdint>
#include <cstddef>

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

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

    vec3(float x, float y, float z) : x{x}, y{y}, z{z} {}
    operator ioVec() const { return ioVec{x, y, z, 0}; }
};
