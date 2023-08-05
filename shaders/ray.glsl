struct Ray
{
    vec3 origin;
    vec3 direction;
};

vec3 Ray_at(Ray r, float pos)
{
    return r.origin + pos * r.direction;
}
