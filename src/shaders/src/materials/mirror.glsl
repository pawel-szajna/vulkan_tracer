#pragma once

#include "../types/collision.glsl"
#include "../types/ray.glsl"
#include "../types/reflection.glsl"
#include "../utils/random.glsl"
#include "../utils/vector_ops.glsl"

ReflectionOpt Mirror_reflect(Ray ray, CollisionOpt collision)
{
    vec3 reflected = reflection(normalize(ray.direction), collision.normal);
    if (dot(reflected, collision.normal) <= 0)
    {
        return InvalidReflection;
    }
    uint vectorId = inputs.materials[collision.material].y;
    return ReflectionOpt(true,
                         Ray(collision.point, reflected, ray.wavelength),
                         colorIntensity(ray.wavelength, inputs.vectors[vectorId].xyz));
}
