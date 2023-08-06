ReflectionOpt Scene_reflect(Ray ray, CollisionOpt collision)
{
    switch (inputs.materials[collision.material])
    {
    case MaterialType_Diffuse:
        return Diffuse_reflect(ray, collision);
    case MaterialType_Mirror:
        return Mirror_reflect(ray, collision);
    case MaterialType_Fog:
        return Fog_reflect(ray, collision);
    case MaterialType_Light:
        return InvalidReflection;
    default:
        break;
    }

    return InvalidReflection;
}

float Scene_shine(float wavelength, int material)
{
    switch(inputs.materials[material])
    {
    case MaterialType_Light:
        return Light_shine(wavelength, material);
    default:
        return 1.0;
    }
}

CollisionOpt Scene_hit(Ray ray, DataUsage usage, float min, float max)
{
    CollisionOpt closest = NoCollision;

    uint vectorId = usage.vectors;
    uint scalarId = usage.scalars;
    uint integerId = usage.integers;

    for (uint i = 0; i < inputs.shapesCount; ++i)
    {
        CollisionOpt current = NoCollision;

        switch (inputs.shapes[i])
        {
        case ShapeType_Sphere:
            {
                vec3 center = inputs.vectors[vectorId].xyz;
                float radius = inputs.scalars[scalarId];
                int material = inputs.integers[integerId];
                current = Sphere_hit(center, radius, material, ray, min, closest.position);
                vectorId += 1;
                scalarId += 1;
                integerId += 1;
                break;
            }
        case ShapeType_Cloud:
            {
                vec3 center = inputs.vectors[vectorId].xyz;
                float radius = inputs.scalars[scalarId];
                float intensity = inputs.scalars[scalarId+1];
                int material = inputs.integers[integerId];
                current = Cloud_hit(center, radius, intensity, material, ray, min, closest.position);
                vectorId += 1;
                scalarId += 2;
                integerId += 1;
                break;
            }
        default:
            break;
        }

        if (current.valid && current.position < closest.position)
        {
            closest = current;
        }
    }

    return closest;
}
