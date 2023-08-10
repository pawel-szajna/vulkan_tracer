#pragma once
#include "../materials/material.glsl"
#include "../types/collision.glsl"
#include "../types/ray.glsl"
#include "../utils/random.glsl"

ReflectionOpt Fog_reflect(Ray ray, CollisionOpt collision)
{
    DataUsage usage = materialsDataUsage(collision.material);
    return ReflectionOpt(true,
                         Ray(collision.point, randomVector(-1, 1), ray.wavelength),
                         inputs.scalars[usage.scalars]);
}
