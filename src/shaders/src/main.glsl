#version 430

#include generated constants.glsl
#include generated io_types.glsl

#include "scene.glsl"
#include "trace.glsl"

#include "utils/random.glsl"
#include "utils/colors.glsl"
#include "utils/cie1931.glsl"
#include "utils/debug.glsl"

#include "glsl_builtin.h"

void main()
{
    uint x = gl_GlobalInvocationID.x + inputs.offsetX;
    uint y = gl_GlobalInvocationID.y + inputs.offsetY;
    initializeRng(x, y);

    uint samples = inputs.samplesPerShader;
    DataUsage usage = materialsDataUsage(inputs.materialsCount);

    float height = tan(0.5 * radians(inputs.renderHeight * inputs.cameraFov / inputs.renderHeight));
    float width = height * inputs.renderWidth / inputs.renderHeight;

    vec3 camW = normalize(inputs.cameraOrigin.xyz - inputs.cameraTarget.xyz);
    vec3 camU = normalize(cross(inputs.cameraUp.xyz, camW));
    vec3 camV = cross(camW, camU);

    vec3 horizontal = camU * width;
    vec3 vertical = camV * height;

    vec3 screenStart = inputs.cameraOrigin.xyz - (0.5 * horizontal) - (0.5 * vertical) - camW;

    vec3 pixel = vec3(0, 0, 0);

    for (uint i = 0; i < samples; ++i)
    {
        float sceneX = float(x + random()) / inputs.renderWidth;
        float sceneY = float(y + random()) / inputs.renderHeight;
        Ray ray = {
            inputs.cameraOrigin.xyz,
            screenStart + horizontal * sceneX + vertical * sceneY - inputs.cameraOrigin.xyz,
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
