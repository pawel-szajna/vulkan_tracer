#pragma once
#include "../glsl_builtin.h"

struct Ray
{
    vec3 origin;
    vec3 direction;
    float wavelength;
};

const Ray InvalidRay = Ray(vec3(0, 0, 0), vec3(0, 0, 0), 0);

vec3 Ray_at(Ray r, float pos)
{
    return r.origin + pos * r.direction;
}
