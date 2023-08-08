#pragma once
#include "../types/collision.glsl"
#include "../types/ray.glsl"

CollisionOpt Sphere_hit(vec3 sphereCenter, float sphereRadius, int sphereMaterial,
                        Ray ray, float min, float max)
{
    vec3 oc = ray.origin - sphereCenter;

    float a = dot(ray.direction, ray.direction);
    float b = dot(oc, ray.direction);
    float c = dot(oc, oc) - sphereRadius * sphereRadius;
    float d = b * b - a * c;

    if (d <= 0 || (a > -1e-6 && a < 1e-6))
    {
        return NoCollision;
    }

    float dSqrt = sqrt(d);

    float root = (-b-dSqrt)/a;

    if (root < min || root > max)
    {
        root = (-b+dSqrt)/a;
    }

    if (root < min || root > max)
    {
        return NoCollision;
    }

    vec3 point = Ray_at(ray, root);

    return Collision(ray.direction,
                     root,
                     point,
                     (point - sphereCenter) / sphereRadius,
                     sphereMaterial);

}
