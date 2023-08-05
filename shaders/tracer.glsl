#version 430

const uint shapesLimit = 16;
const uint vectorsLimit = 32;
const uint scalarsLimit = 32;

const uint ShapeType_Sphere = 1;

layout(set = 0, binding = 0, std430)
buffer Input
{
    uint randomSeed;
    uint samplesPerShader;

    uint shapeCount;
    uint shapes[shapesLimit];

    vec4 vectors[vectorsLimit];
    uint scalars[scalarsLimit];
}
inputs;

layout(set = 0, binding = 1, std430)
buffer Output
{
    vec4 pixels[];
}
results;

const uint renderWidth = 640;
const uint renderHeight = 480;

uint randomInt()
{
    uint x = inputs.randomSeed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    inputs.randomSeed = x;
    return x;
}

float random()
{
    return randomInt() / 4294967295.0;
}

struct Ray
{
    vec3 origin;
    vec3 direction;
};

vec3 Ray_at(Ray r, float pos)
{
    return r.origin + pos * r.direction;
}

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

CollisionOpt Sphere_hit(vec3 sphereCenter, float sphereRadius, Ray ray, float min, float max)
{
    vec3 oc = ray.origin - sphereCenter;

    float a = dot(ray.direction, ray.direction);
    float b = dot(oc, ray.direction);
    float c = dot(oc, oc) - sphereRadius * sphereRadius;
    float d = b * b - a * c;

    if (d <= 0)
    {
        return NoCollision;
    }

    float dSqrt = sqrt(d);
    float root = (-b-dSqrt)/a;
    if (root < min || root > max)
    {
        root = (-b+dSqrt)/a;
        if (root < min || root > max)
        {
            return NoCollision;
        }
    }
    vec3 point = Ray_at(ray, root);
    return Collision(ray.direction, root, point, (point - sphereCenter) / sphereRadius);
}

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

void initializeRng(uint x, uint y)
{
    inputs.randomSeed += randomInt() % (((x * 74567 - 1) * randomInt()) << 16 + ((y + 102761) * randomInt()));
    inputs.randomSeed *= randomInt() % (((y * 47981 - 1) * randomInt()) << 16 + ((x +  60257) * randomInt()));
}

void main()
{
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    initializeRng(x, y);

    uint samples = inputs.samplesPerShader;
    float weight = 1.0 / samples;

    vec3 screenStart = vec3(-0.5, -0.375, -1.0);

    Ray ray = {
        vec3(0.0, 0.0, 0.0),
        screenStart + vec3(float(x) / renderWidth, float(y) / renderWidth, 0.0)
    };

    for (uint i = 0; i < samples; ++i)
    {
        results.pixels[x + renderWidth * y] += weight * trace(ray);
    }
}
