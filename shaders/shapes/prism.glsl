#pragma once
#include "../types/collision.glsl"
#include "../types/ray.glsl"

struct PrismWallIntersection
{
    bool valid;
    vec3 normal;
    float distance;
    float position;
};

const PrismWallIntersection NoPrismWallIntersection =
    PrismWallIntersection(false, vec3(0, 0, 0), 1.0 / 0.0, 1.0 / 0.0);

float Prism_distance2D(vec2 a, vec2 b)
{
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

bool Prism_between2D(vec2 a, vec2 b, vec2 p)
{
    return abs(Prism_distance2D(a, p) + Prism_distance2D(p, b) - Prism_distance2D(a, b)) < 1e-9;
}

PrismWallIntersection Prism_intersectWall(Ray ray, vec2 segment1, vec2 segment2)
{
    vec2 ray1 = ray.origin.xz;
    vec2 ray2 = (ray.origin + ray.direction).xz;

    float denom = (segment1.x - segment2.x) * (ray1.y - ray2.y)
                - (segment1.y - segment2.y) * (ray1.x - ray2.x);

    if (denom == 0)
    {
        return NoPrismWallIntersection;
    }

    denom = 1.0 / denom;

    float A = segment1.x * segment2.y - segment1.y * segment2.x;
    float B = ray1.x * ray2.y - ray1.y * ray2.x;

    vec2 intersectionPoint = vec2(denom * (A * (ray1.x - ray2.x) - B * (segment1.x - segment2.x)),
                                  denom * (A * (ray1.y - ray2.y) - B * (segment1.y - segment2.y)));

    float distance = Prism_distance2D(intersectionPoint, ray1);

    if (abs(distance) < 1e-6)
    {
        return NoPrismWallIntersection;
    }

    if (!Prism_between2D(segment1, segment2, intersectionPoint))
    {
        return NoPrismWallIntersection;
    }

    vec3 normal = -normalize(vec3(segment2.y - segment1.y, 0, segment1.x - segment2.x));
    float position = (intersectionPoint.x - ray.origin.x) / ray.direction.x;

    return PrismWallIntersection(true, normal, distance, position);
}

CollisionOpt Prism_collisionFromOutside(Ray ray,
                                        float top, float bottom,
                                        int material,
                                        PrismWallIntersection xsect1,
                                        PrismWallIntersection xsect2,
                                        float min, float max)
{
//    if (!(xsect1.valid && xsect2.valid))
//    {
//        return NoCollision;
//    }

    vec3 hit = Ray_at(ray, xsect1.position);

//    if (hit.y > top)
//    {
//        if (ray.direction.y > 0)
//        {
//            return NoCollision;
//        }
//
//        xsect1.position = (top - ray.origin.y) / ray.direction.y;
//
//        if (xsect2.position < xsect1.position)
//        {
//            return NoCollision;
//        }
//
//        xsect1.normal = vec3(0, 1, 0);
//    }
//
//    if (hit.y < bottom)
//    {
//        if (ray.direction.y < 0)
//        {
//            return NoCollision;
//        }
//
//        xsect1.position = (bottom - ray.origin.y) / ray.direction.y;
//
//        if (xsect2.position < xsect1.position)
//        {
//            return NoCollision;
//        }
//
//        xsect1.normal = vec3(0, -1, 0);
//    }
//
    if (xsect1.position < min || xsect1.position > max)
    {
        return NoCollision;
    }

    CollisionOpt collision =
        Collision(ray.direction, xsect1.position, Ray_at(ray, xsect1.position), xsect1.normal, material);

//    if (collision.fromInside)
//    {
//        return NoCollision;
//    }

    return collision;
}

CollisionOpt Prism_collisionFromInside()
{
    return NoCollision;
}

CollisionOpt Prism_hit(float top, float bottom,
                       int vertices,
                       uint vectorId,
                       int material,
                       Ray ray, float min, float max)
{
    if ((ray.origin.y < bottom && ray.direction.y < 0) ||
        (ray.origin.y > top && ray.direction.y > 0))
    {
        return NoCollision;
    }

    PrismWallIntersection xsect1 = NoPrismWallIntersection;
    PrismWallIntersection xsect2 = NoPrismWallIntersection;

    uint validIntersections = 0;

    for (int i = 0; i < vertices; ++i)
    {
        PrismWallIntersection current =
            Prism_intersectWall(ray,
                                inputs.vectors[vectorId + i].xz,
                                inputs.vectors[vectorId + ((i + 1) % vertices)].xz);

        if (!current.valid)
        {
            continue;
        }

        ++validIntersections;

        if (current.distance < xsect1.distance)
        {
            xsect2 = xsect1;
            xsect1 = current;
        }
        else if (current.distance < xsect2.distance)
        {
            xsect2 = current;
        }
    }

    if (validIntersections == 0)
    {
        return NoCollision;
    }

    return validIntersections % 2 == 1
         ? Prism_collisionFromInside()
         : Prism_collisionFromOutside(ray, top, bottom, material, xsect1, xsect2, min, max);
}
