struct CollisionOpt
{
    bool valid;
    float position;
    vec3 point;
    vec3 normal;
    bool fromInside;
};

const CollisionOpt NoCollision = CollisionOpt(false, 1.0 / 0.0, vec3(0, 0, 0), vec3(0, 0, 0), false);
const CollisionOpt DebugCollision = CollisionOpt(true, 1, vec3(1, 1, 1), vec3(1, 1, 1), false);
CollisionOpt Collision(vec3 direction, float position, vec3 point, vec3 normal)
{
    bool fromInside = dot(direction, normal) >= 0;
    if (fromInside)
    {
        normal = -normal;
    }
    return CollisionOpt(true, position, point, normal, fromInside);
}
