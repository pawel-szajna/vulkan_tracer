CollisionOpt Scene_hit(Ray ray, float min, float max)
{
    CollisionOpt closest = NoCollision;

    uint shapeId = 0;
    uint vectorId = 0;
    uint scalarId = 0;

    for (uint i = 0; i < inputs.shapeCount; ++i)
    {
        CollisionOpt current = NoCollision;

        switch (inputs.shapes[i])
        {
            case ShapeType_Sphere:
            current = Sphere_hit(
            inputs.vectors[vectorId].xyz,
            inputs.scalars[scalarId],
            ray, min, max);
            vectorId += 1;
            scalarId += 1;
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
