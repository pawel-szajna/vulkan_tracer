layout(set = 0, binding = 0, std430)
buffer Input
{
    uint randomSeed;
    uint samplesPerShader;

    uint width;
    uint height;

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
