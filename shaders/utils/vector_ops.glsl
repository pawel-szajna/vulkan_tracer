#pragma once
#include "../glsl_builtin.h"

vec3 reflection(vec3 vector, vec3 normal)
{
    return vector - 2 * normal * dot(vector, normal);
}
