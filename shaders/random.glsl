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

void initializeRng(uint x, uint y)
{
    inputs.randomSeed += randomInt() % (((x * 74567 - 1) * randomInt()) << 16 + ((y + 102761) * randomInt()));
    inputs.randomSeed *= randomInt() % (((y * 47981 - 1) * randomInt()) << 16 + ((x +  60257) * randomInt()));
}
