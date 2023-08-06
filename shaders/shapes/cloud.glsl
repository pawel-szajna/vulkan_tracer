CollisionOpt Cloud_hit(vec3 center, float radius,
                       float density,
                       int material,
                       Ray ray, float pmin, float pmax)
{
    CollisionOpt cloudEnterHit =
        Sphere_hit(center, radius, material, ray, -1.0 / 0.0, 1.0 / 0.0);

    if (!cloudEnterHit.valid)
    {
        return NoCollision;
    }

    CollisionOpt cloudExitHit =
        Sphere_hit(center, radius, material, ray, cloudEnterHit.position + 1e-4, 1.0 / 0.0);

    if (!cloudExitHit.valid)
    {
        return NoCollision;
    }

    cloudEnterHit.position = max(cloudEnterHit.position, pmin);
    cloudExitHit.position = min(cloudExitHit.position, pmax);

    if (cloudEnterHit.position >= cloudExitHit.position)
    {
        return NoCollision;
    }

    cloudEnterHit.position = max(cloudEnterHit.position, 0);

    float rayLength = length(ray.direction);
    float insideRayDistance = (cloudExitHit.position - cloudEnterHit.position) * rayLength;
    float collisionPoint = density * log(random());

    if (collisionPoint > insideRayDistance)
    {
        return NoCollision;
    }

    float rayPosition = cloudEnterHit.position + collisionPoint / rayLength;

    return Collision(ray.direction,
                     rayPosition,
                     Ray_at(ray, rayPosition),
                     vec3(1, 0, 0),
                     material);
}
