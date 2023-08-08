#pragma once
#include "../materials/material.glsl"
#include "../types/collision.glsl"
#include "../types/ray.glsl"
#include "../utils/random.glsl"
#include "../utils/vector_ops.glsl"

const float Glass_refractionFactorA = 1.33442e7;
const float Glass_refractionFactorB = -3102.98;

vec3 refract(vec3 direction, vec3 normal, float cosTheta, float ratio)
{
    vec3 perpendicular = ratio * (direction + cosTheta * normal);
    float perpendicularLengthSquared = perpendicular.x * perpendicular.x
                                     + perpendicular.y * perpendicular.y
                                     + perpendicular.z * perpendicular.z;
    vec3 parallel = normal * (-sqrt(abs(1 - perpendicularLengthSquared)));
    return parallel + perpendicular;
}

float reflectance(float cosTheta, float refractionRatio)
{
    // https://en.wikipedia.org/wiki/Schlick%27s_approximation
    float r = (1 - refractionRatio) / (1 + refractionRatio);
    return r + (1 - r) * pow(1 - cosTheta, 5);
}

float refractionOutgoing(float refractiveIndex, float wavelength)
{
    float m = (wavelength - Glass_refractionFactorB) * (wavelength - Glass_refractionFactorB);
    return refractiveIndex * Glass_refractionFactorA / m;
}

float refractionIncoming(float refractiveIndex, float wavelength)
{
    float m = (wavelength - Glass_refractionFactorB) * (wavelength - Glass_refractionFactorB);
    return m / (refractiveIndex * Glass_refractionFactorA);
}

ReflectionOpt Glass_reflect(Ray ray, CollisionOpt collision)
{
    DataUsage usage = materialsDataUsage(collision.material);
    float refractiveIndex = inputs.scalars[usage.scalars];

    float ratio = collision.fromInside
                ? refractionOutgoing(refractiveIndex, ray.wavelength)
                : refractionIncoming(refractiveIndex, ray.wavelength);

    vec3 dirUnit = normalize(ray.direction);
    float cosTheta = min(1, dot(-dirUnit, collision.normal));
    float sinTheta = sqrt(1 - cosTheta * cosTheta);

    if (ratio * sinTheta > 1 || reflectance(cosTheta, ratio) > random())
    {
        return ReflectionOpt(true,
                             Ray(collision.point, reflection(dirUnit, collision.normal), ray.wavelength),
                             1);
    }

    return ReflectionOpt(true,
                         Ray(collision.point,
                             refract(dirUnit, collision.normal, cosTheta, ratio),
                             ray.wavelength),
                         1);
}
