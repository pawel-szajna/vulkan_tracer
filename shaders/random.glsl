uint randomInt()
{
    uint state = inputs.randomSeed;
    inputs.randomSeed = inputs.randomSeed * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float random()
{
    return randomInt() / 4294967295.0;
}

float random(float min, float max)
{
    return min + random() * (max - min);
}

vec3 randomVector(float min, float max)
{
    return vec3(random(min, max), random(min, max), random(min, max));
}

vec3 randomUnit()
{
    return normalize(randomVector(-1, 1));
}

void initializeRng(uint x, uint y)
{
    inputs.randomSeed += x + inputs.width * y;
}
