#pragma once

#include "basic_types.hpp"
#include "constants.hpp"

struct InputData
{
    u32 randomSeed{};
    u32 samplesPerShader{1};

    u32 shapesCount{};
    u32 shapes[shapesLimit];

    float vectors[vectorsLimit * 4];
    float scalars[scalarsLimit];
};

constexpr usize inputSize = sizeof(InputData);

struct OutputData
{
    float pixels[renderWidth * renderHeight];
};

constexpr usize outputSize = sizeof(OutputData);
