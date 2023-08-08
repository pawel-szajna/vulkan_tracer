#pragma once
#include "../materials/material.glsl"
#include "../types/collision.glsl"
#include "../types/ray.glsl"
#include "../utils/random.glsl"
#include "../utils/colors.glsl"

ReflectionOpt Diffuse_reflect(Ray ray, CollisionOpt collision)
{
    DataUsage usage = materialsDataUsage(collision.material);
    return ReflectionOpt(true,
                         Ray(collision.point, collision.normal + randomInSphere(), ray.wavelength),
                         colorIntensity(ray.wavelength, inputs.vectors[usage.vectors].xyz));
}
