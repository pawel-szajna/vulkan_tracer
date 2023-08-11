#pragma once

#include "types/collision.glsl"
#include "types/ray.glsl"
#include "types/reflection.glsl"
#include "utils/random.glsl"

float trace(Ray ray)
{
    float intensity = 1.0;

    for (uint i = 0; i < inputs.reflectionsLimit; ++i)
    {
        CollisionOpt collision = Scene_hit(ray, 0.001, 1.0 / 0.0);
        if (!collision.valid)
        {
            return intensity * inputs.background.w * colorIntensity(ray.wavelength, inputs.background.xyz);
        }

        if (collision.material < 0)
        {
            return random();
        }

        ReflectionOpt reflection = Scene_reflect(ray, collision);
        float lighting = Scene_shine(ray.wavelength, collision.material);
        intensity *= lighting;
        if (!reflection.valid)
        {
            return intensity;
        }
        ray = reflection.ray;
        intensity *= reflection.intensity;
    }

    return intensity;
}
