#pragma once
#include "../materials/material.glsl"
#include "../types/collision.glsl"
#include "../types/ray.glsl"
#include "../utils/random.glsl"

float Light_shine(float wavelength, int material)
{
    DataUsage usage = materialsDataUsage(material);
    return inputs.scalars[usage.scalars] * colorIntensity(wavelength, inputs.vectors[usage.vectors].xyz);
}
