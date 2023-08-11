#pragma once
#include "../types/collision.glsl"
#include "../types/ray.glsl"
#include "../types/reflection.glsl"
#include "../utils/random.glsl"

ReflectionOpt Fog_reflect(Ray ray, CollisionOpt collision)
{
    uint scalarId = inputs.materials[collision.material].z;
    return ReflectionOpt(true,
                         Ray(collision.point, randomVector(-1, 1), ray.wavelength),
                         inputs.scalars[scalarId]);
}
