float trace(Ray ray, DataUsage usage)
{
    float intensity = 1.0;

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
            intensity *= reflection.intensity;
            continue;
        }

        vec3 unitDir = normalize(ray.direction);
        float t = 0.5 * (unitDir.y + 1.0);
        intensity *= (1.0 - t) * 1 + t * 0.5;
        break;
    }

    return intensity;
}
