#version 430

#include generated constants
#include generated io_types

#include utils/random

#include types/ray
#include types/collision

#include shapes/sphere

#include materials/diffuse
#include materials/mirror

#include scene
#include trace

#include utils/debug

void main()
{
    uint x = gl_GlobalInvocationID.x;
    uint y = gl_GlobalInvocationID.y;
    initializeRng(x, y);

    uint samples = inputs.samplesPerShader;
    float weight = 1.0 / samples;

    vec3 screenStart = vec3(-0.5, -0.375, -1.0);
    DataUsage usage = materialsDataUsage(inputs.materialsCount);

    for (uint i = 0; i < samples; ++i)
    {
        Ray ray = {
            vec3(0.0, 0.0, 0.0),
            screenStart + vec3(float(x + random()) / inputs.renderWidth,
                               float(y + random()) / inputs.renderWidth,
                               0.0)
        };

        results.pixels[x + inputs.renderWidth * y] += weight * trace(ray, usage);
    }

    debug(x, y);
}
