#include materials/material

ReflectionOpt Fog_reflect(Ray ray, CollisionOpt collision)
{
    DataUsage usage = materialsDataUsage(collision.material);
    return ReflectionOpt(true,
                         Ray(collision.point, randomVector(-1, 1), ray.wavelength),
                         inputs.scalars[usage.scalars]);
}
