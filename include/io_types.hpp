#pragma once

#include "basic_types.hpp"
#include "constants.hpp"

struct InputData
{
    u32 randomSeed;
    u32 samplesPerShader;

    u32 renderWidth;
    u32 renderHeight;

    u32 shapesCount;
    u32 shapes[shapesLimit];

    u8 padding[16 - ((5 + shapesLimit) * 4) % 16];

    float vectors[vectorsLimit * 4];
    float scalars[scalarsLimit];
};

constexpr usize inputSize = sizeof(InputData);

struct OutputData
{
    float pixels[renderWidth * renderHeight * 4];
};

constexpr usize outputSize = sizeof(OutputData);
