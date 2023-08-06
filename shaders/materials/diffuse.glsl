#include materials/material

ReflectionOpt Diffuse_reflect(Ray ray, CollisionOpt collision)
{
    DataUsage usage = materialsDataUsage(collision.material);
    return ReflectionOpt(true,
                         Ray(collision.point, collision.normal + randomInSphere()),
                         inputs.vectors[usage.vectors]);
}
