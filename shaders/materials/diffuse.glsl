#include materials/material

Reflection Diffuse_reflect(Ray ray, CollisionOpt collision)
{
    DataUsage usage = materialsDataUsage(collision.material);
    return Reflection(Ray(collision.point, collision.normal + randomInSphere()),
                      inputs.vectors[usage.vectors]);
}
