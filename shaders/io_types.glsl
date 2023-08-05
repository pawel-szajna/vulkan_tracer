layout(set = 0, binding = 0, std430)
buffer Input                        // size    align   begin   end
{
    uint randomSeed;                // 4       4       0       4
    uint samplesPerShader;          // 4       4       4       8
    uint reflectionsLimit;          // 4       4       8       12

    uint width;                     // 4       4       12      16
    uint height;                    // 4       4       16      20

    uint shapeCount;                // 4       4       20      24
    uint shapes[shapesLimit];       // 64      4       24      88

    uint materialCount;             // 4       4       88      92
    uint materials[materialsLimit]; // 32      4       92      124

    vec4 vectors[vectorsLimit];     // 512     16      128(+4) 640
    float scalars[scalarsLimit];    // 128     4       640     768
    int integers[integersLimit];    // 128     4       768     896
}
inputs;

layout(set = 0, binding = 1, std430)
buffer Output
{
    vec4 pixels[];
}
results;
