#include materials/material

ReflectionOpt Mirror_reflect(Ray ray, CollisionOpt collision)
{
    vec3 reflected = reflection(normalize(ray.direction), collision.normal);
    if (dot(reflected, collision.normal) <= 0)
    {
        return InvalidReflection;
    }
    DataUsage usage = materialsDataUsage(collision.material);
    return ReflectionOpt(true,
                         Ray(collision.point, reflected, ray.wavelength),
                         colorIntensity(ray.wavelength, inputs.vectors[usage.vectors].xyz));
}
