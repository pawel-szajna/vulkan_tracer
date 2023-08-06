#include materials/material

vec3 reflection(vec3 vector, vec3 normal)
{
    return vector - 2 * normal * dot(vector, normal);
}

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
