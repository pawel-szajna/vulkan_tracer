vec4 trace(Ray ray, DataUsage usage)
{
    vec4 color = vec4(0, 0, 0, 0);
    float multiplier = 1;

    for (uint i = 0; i < inputs.reflectionsLimit; ++i)
    {
        CollisionOpt collision = Scene_hit(ray, usage, 0.001, 1.0 / 0.0);
        if (collision.valid)
        {
            vec3 target = collision.point + collision.normal + randomInSphere();
            multiplier *= 0.5;
            ray.origin = collision.point;
            ray.direction = target - collision.point;
            continue;
        }

        vec3 unitDir = normalize(ray.direction);
        float t = 0.5 * (unitDir.y + 1.0);
        color = (1.0 - t) * vec4(1, 1, 1, 0) + t * vec4(0.5, 0.7, 1.0, 0);
        break;
    }

    return multiplier * color;
}
