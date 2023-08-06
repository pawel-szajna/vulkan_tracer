const uint Diffuse_usageVectors = 1;
const uint Diffuse_usageScalars = 0;
const uint Diffuse_usageIntegers = 0;

const uint Mirror_usageVectors = 1;
const uint Mirror_usageScalars = 0;
const uint Mirror_usageIntegers = 0;

struct DataUsage
{
    uint vectors;
    uint scalars;
    uint integers;
};

struct ReflectionOpt
{
    bool valid;
    Ray ray;
    float intensity;
};

const ReflectionOpt InvalidReflection = ReflectionOpt(false, InvalidRay, 0);

DataUsage materialsDataUsage(uint limit)
{
    DataUsage results = DataUsage(0, 0, 0);

    for (uint i = 0; i < limit; ++i)
    {
        switch(inputs.materials[i])
        {
        case MaterialType_Diffuse:
            results.vectors += Diffuse_usageVectors;
            results.scalars += Diffuse_usageScalars;
            results.integers += Diffuse_usageIntegers;
            break;
        case MaterialType_Mirror:
            results.vectors += Mirror_usageVectors;
            results.scalars += Mirror_usageScalars;
            results.integers += Mirror_usageIntegers;
            break;
        default:
            break;
        }
    }

    return results;
}
