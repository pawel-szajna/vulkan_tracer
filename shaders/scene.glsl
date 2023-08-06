CollisionOpt Scene_hit(Ray ray, DataUsage usage, float min, float max)
{
    CollisionOpt closest = NoCollision;

    uint shapeId = 0;
    uint vectorId = usage.vectors;
    uint scalarId = usage.scalars;
    uint integerId = usage.integers;

    for (uint i = 0; i < inputs.shapesCount; ++i)
    {
        CollisionOpt current = NoCollision;

        switch (inputs.shapes[i])
        {
        case ShapeType_Sphere:
            vec3 center = inputs.vectors[vectorId].xyz;
            float radius = inputs.scalars[scalarId];
            int material = inputs.integers[integerId];
            current = Sphere_hit(center, radius, material, ray, min, max);
            vectorId += 1;
            scalarId += 1;
            integerId += 1;
            break;
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
