float trace(Ray ray, DataUsage usage)
{
    float intensity = 1.0;

    for (uint i = 0; i < inputs.reflectionsLimit; ++i)
    {
        CollisionOpt collision = Scene_hit(ray, usage, 0.001, 1.0 / 0.0);
        if (collision.valid)
        {
            ReflectionOpt reflection = Scene_reflect(ray, collision);
            float lighting = Scene_shine(ray.wavelength, collision.material);
            intensity *= lighting;
            if (!reflection.valid)
            {
                return intensity;
            }
            ray = reflection.ray;
            intensity *= reflection.intensity;
            continue;
        }

        return intensity * inputs.background.w * colorIntensity(ray.wavelength, inputs.background.xyz);
    }

    return intensity;
}
