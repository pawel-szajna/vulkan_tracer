#pragma once

#include <types/Basic.hpp>

namespace vrt::scene
{
enum class MaterialType : u32
{
    Invalid,
    Diffuse,
    Mirror,
    Fog,
    Light,
    Glass,
};
}
