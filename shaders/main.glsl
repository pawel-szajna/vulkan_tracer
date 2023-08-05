#version 430

#include constants
#include io_types
#include random

#include ray
#include collision

#include sphere

#include scene
#include trace
#include debug

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
        screenStart + vec3(float(x) / inputs.width, float(y) / inputs.width, 0.0)
    };

    for (uint i = 0; i < samples; ++i)
    {
        results.pixels[x + inputs.width * y] += weight * trace(ray);
    }

    debug(x, y);
}
