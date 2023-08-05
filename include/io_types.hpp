#pragma once

#include "basic_types.hpp"
#include "constants.hpp"

struct InputData
{
    u32 randomSeed;
    u32 samplesPerShader;
    u32 reflectionsLimit;

    u32 renderWidth;
    u32 renderHeight;

    u32 shapesCount;
    u32 shapes[shapesLimit];

    u32 materialCount;
    u32 materials[materialsLimit];

    u8 padding[16 - ((7 + shapesLimit + materialsLimit) * 4) % 16];

    float vectors[vectorsLimit * 4];
    float scalars[scalarsLimit];
    i32 integers[integersLimit];
};

constexpr usize inputSize = sizeof(InputData);
