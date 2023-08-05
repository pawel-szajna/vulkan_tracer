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
    inputs.randomSeed += randomInt() % (((x * 74567 - 1) * randomInt()) << 16 + ((y + 102761) * randomInt()));
    inputs.randomSeed *= randomInt() % (((y * 47981 - 1) * randomInt()) << 16 + ((x +  60257) * randomInt()));
}
