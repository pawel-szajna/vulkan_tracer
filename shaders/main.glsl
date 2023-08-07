#version 430

#include generated constants
#include generated io_types

#include utils/random
#include utils/colors
#include utils/vector_ops

#include types/ray
#include types/collision

#include shapes/sphere
#include shapes/cloud

#include materials/diffuse
#include materials/glass
#include materials/light
#include materials/fog
#include materials/mirror

#include scene
#include trace

#include utils/cie1931
#include utils/debug

void main()
{
    uint x = gl_GlobalInvocationID.x + inputs.offsetX;
    uint y = gl_GlobalInvocationID.y + inputs.offsetY;
    initializeRng(x, y);

    uint samples = inputs.samplesPerShader;

    vec3 screenStart =
        vec3(-0.5, -0.5 * (float(inputs.renderHeight) / float(inputs.renderWidth)), -1.0);
    DataUsage usage = materialsDataUsage(inputs.materialsCount);

    vec3 pixel = vec3(0, 0, 0);

    for (uint i = 0; i < samples; ++i)
    {
        Ray ray = {
            vec3(0.0, 0.0, 0.0),
            screenStart + vec3(float(x + random()) / inputs.renderWidth,
                               float(y + random()) / inputs.renderWidth,
                               0.0),
            random(380, 780)
        };

        pixel += inputs.weight
               * trace(ray, usage)
               * vec3(colorSpaceX(ray.wavelength),
                      colorSpaceY(ray.wavelength),
                      colorSpaceZ(ray.wavelength));
    }

    results.pixels[x + inputs.renderWidth * y] += vec4(pixel, 0);
}
