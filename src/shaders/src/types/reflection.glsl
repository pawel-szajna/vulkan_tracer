#pragma once
#include "../types/ray.glsl"

struct ReflectionOpt
{
    bool valid;
    Ray ray;
    float intensity;
};

const ReflectionOpt InvalidReflection = { false, InvalidRay, 0 };
