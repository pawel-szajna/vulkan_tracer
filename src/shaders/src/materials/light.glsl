#pragma once

#include "../types/collision.glsl"
#include "../types/ray.glsl"
#include "../types/reflection.glsl"
#include "../utils/random.glsl"

float Light_shine(float wavelength, int material)
{
    uint vectorId = inputs.materials[material].y;
    uint scalarId = inputs.materials[material].z;

    return inputs.scalars[scalarId] * colorIntensity(wavelength, inputs.vectors[vectorId].xyz);
}
