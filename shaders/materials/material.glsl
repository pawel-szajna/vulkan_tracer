const uint Diffuse_usageVectors = 1;
const uint Diffuse_usageScalars = 0;
const uint Diffuse_usageIntegers = 0;

struct DataUsage
{
    uint vectors;
    uint scalars;
    uint integers;
};

struct Reflection
{
    Ray ray;
    vec4 color;
};

const Reflection InvalidReflection = Reflection(InvalidRay, vec4(0, 0, 0, 0));

DataUsage materialsDataUsage(uint limit)
{
    DataUsage results = DataUsage(0, 0, 0);

    for (uint i = 0; i < limit; ++i)
    {
        switch(inputs.materials[i])
        {
        case MaterialType_Diffuse:
            results.vectors += Diffuse_usageVectors;
            break;
        default:
            break;
        }
    }

    return results;
}
