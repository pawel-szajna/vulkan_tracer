vec4 trace(Ray ray, DataUsage usage)
{
    vec4 color = vec4(1, 1, 1, 0);
    float multiplier = 1.0;

    for (uint i = 0; i < inputs.reflectionsLimit; ++i)
    {
        CollisionOpt collision = Scene_hit(ray, usage, 0.001, 1.0 / 0.0);
        if (collision.valid)
        {
            ReflectionOpt reflection = Scene_reflect(ray, collision);
            if (!reflection.valid)
            {
                break;
            }
            ray = reflection.ray;
            color *= reflection.color;
            continue;
        }

        vec3 unitDir = normalize(ray.direction);
        float t = 0.5 * (unitDir.y + 1.0);
        color *= (1.0 - t) * vec4(1, 1, 1, 0) + t * vec4(0.5, 0.7, 1.0, 0);
        break;
    }

    return multiplier * color;
}
