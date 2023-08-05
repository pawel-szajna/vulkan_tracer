vec4 trace(Ray ray)
{
    CollisionOpt c = Scene_hit(ray, 0, 1.0 / 0.0);
    if (c.valid)
    {
        return vec4(random(), random(), random(), 0);
    }

    vec3 unitDir = normalize(ray.direction);
    float t = 0.5 * (unitDir.y + 1.0);
    return (1.0 - t) * vec4(1, 1, 1, 0) + t * vec4(0.5, 0.7, 1.0, 0);
}
