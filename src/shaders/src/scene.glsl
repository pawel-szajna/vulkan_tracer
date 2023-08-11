#pragma once

#include "types/ray.glsl"
#include "types/collision.glsl"

#include "materials/diffuse.glsl"
#include "materials/fog.glsl"
#include "materials/glass.glsl"
#include "materials/light.glsl"
#include "materials/mirror.glsl"

#include "shapes/prism.glsl"
#include "shapes/sphere.glsl"
#include "shapes/cloud.glsl"

#include "glsl_builtin.h"

ReflectionOpt Scene_reflect(Ray ray, CollisionOpt collision)
{
    switch (inputs.materials[collision.material].x)
    {
    case MaterialType_Diffuse:
        return Diffuse_reflect(ray, collision);
    case MaterialType_Mirror:
        return Mirror_reflect(ray, collision);
    case MaterialType_Fog:
        return Fog_reflect(ray, collision);
    case MaterialType_Light:
        return InvalidReflection;
    case MaterialType_Glass:
        return Glass_reflect(ray, collision);
    default:
        break;
    }

    return InvalidReflection;
}

float Scene_shine(float wavelength, int material)
{
    switch(inputs.materials[material].x)
    {
    case MaterialType_Light:
        return Light_shine(wavelength, material);
    default:
        return 1.0;
    }
}

CollisionOpt Scene_hit(Ray ray, float min, float max)
{
    CollisionOpt closest = NoCollision;

    for (uint i = 0; i < inputs.shapesCount; ++i)
    {
        CollisionOpt current = NoCollision;

        switch (inputs.shapes[i].x)
        {
        case ShapeType_Sphere:
            {
                vec3 center = inputs.vectors[inputs.shapes[i].y].xyz;
                float radius = inputs.scalars[inputs.shapes[i].z];
                int material = inputs.integers[inputs.shapes[i].w];
                current = Sphere_hit(center, radius, material, ray, min, closest.position);
                break;
            }
        case ShapeType_Cloud:
            {
                vec3 center = inputs.vectors[inputs.shapes[i].x].xyz;
                float radius = inputs.scalars[inputs.shapes[i].z];
                float intensity = inputs.scalars[inputs.shapes[i].z+1];
                int material = inputs.integers[inputs.shapes[i].w];
                current = Cloud_hit(center, radius, intensity, material, ray, min, closest.position);
                break;
            }
        case ShapeType_Prism:
            {
                float top = inputs.scalars[inputs.shapes[i].z];
                float bottom = inputs.scalars[inputs.shapes[i].z+1];
                int vertices = inputs.integers[inputs.shapes[i].w];
                int material = inputs.integers[inputs.shapes[i].w+1];
                current = Prism_hit(top, bottom, vertices, inputs.shapes[i].y, material, ray, min, closest.position);
                break;
            }
        default:
            break;
        }

        if (current.valid && current.position < closest.position)
        {
            closest = current;
        }
    }

    return closest;
}
