layout(set = 0, binding = 0, std430)
buffer Input                      // size    align   begin   end
{
    uint randomSeed;              // 4       4       0       4
    uint samplesPerShader;        // 4       4       4       8

    uint width;                   // 4       4       8       12
    uint height;                  // 4       4       12      16

    uint shapeCount;              // 4       4       16      20
    uint shapes[shapesLimit];     // 64      4       20      84

    vec4 vectors[vectorsLimit];   // 512     16      96      608
    float scalars[scalarsLimit];  // 128     4       608     736
}
inputs;

layout(set = 0, binding = 1, std430)
buffer Output
{
    vec4 pixels[];
}
results;
