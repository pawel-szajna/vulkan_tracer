#version 430

#include generated constants.glsl
#include generated io_types.glsl

#include "utils/random.glsl"
#include "utils/colors.glsl"
#include "utils/vector_ops.glsl"

#include "types/ray.glsl"
#include "types/collision.glsl"

#include "shapes/prism.glsl"
#include "shapes/sphere.glsl"
#include "shapes/cloud.glsl"

#include "materials/diffuse.glsl"
#include "materials/glass.glsl"
#include "materials/light.glsl"
#include "materials/fog.glsl"
#include "materials/mirror.glsl"

#include "scene.glsl"
#include "trace.glsl"

#include "utils/cie1931.glsl"
#include "utils/debug.glsl"

#include "glsl_builtin.h"

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
