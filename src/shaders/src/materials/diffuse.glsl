#pragma once
#include "../types/collision.glsl"
#include "../types/ray.glsl"
#include "../types/reflection.glsl"
#include "../utils/random.glsl"
#include "../utils/colors.glsl"

ReflectionOpt Diffuse_reflect(Ray ray, CollisionOpt collision)
{
    uint vectorId = inputs.materials[collision.material].y;
    return ReflectionOpt(true,
                         Ray(collision.point, collision.normal + randomInSphere(), ray.wavelength),
                         colorIntensity(ray.wavelength, inputs.vectors[vectorId].xyz));
}
