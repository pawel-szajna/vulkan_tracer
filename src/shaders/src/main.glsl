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

    // Nikon DX sensor size: 24x16mm
    // f = (H * WD) / FOV
    // FOV = (H * WD) / f

    // focus distance = 2400 mm
    // qFOV = 60
    // AFOV_H = 80
    // AFOV_H = 1.3962634
    // tan(AFOV_H/2) = 0.83909962981
    // FOV_H = 2 * 2400mm * 0.83909962981
    // FOV_H = 4'027 mm
    // f = (24mm * 2400mm) / 4'027 mm = 57'600 / 4'027 mm = 14.3 mm

// aperture = 150 mm
// f/stop = aperture
// 14.3 mm / stop = 150 mm
// stop = 14.3 / 150 = 0.1

// f/0.1

    // f = 14mm
    // FOV = 80° = 1.396 rad
    // WD = 2400mm
    // H = 24mm
    // f = (H * WD) / FOV
    // FOV = (H * WD) / f
    // FOV = 24mm * 2400mm / 14mm = ~4114 mm

    // AFOV = 2 x atan(H / 2f)
    // AFOV / 2 = atan(H / 2f)
    // tan(AFOV / 2) = H / 2f
    // 2 * f * tan(AFOV/2) = H


    // FOV_H = 2 * WD * tan(FOV) = 2 * 2400mm * 13611mm


    // f = 35mm
    // stop = 1.4
    // aperture = f/1.4 = 25mm
    // AFOV = 2 x atan(H / 2f) = 2*atan(24 mm / 70mm) = 2*atan(0.34285714285) = 2*0.330297355 rad = 0.66059471 rad = 37.8° => 50

    float focusDistance = 2.40;

    float quakeFov = 0.75 * inputs.cameraFov * inputs.renderWidth / inputs.renderHeight;

    float width = 2 * focusDistance * tan(0.5 * radians(quakeFov));
    float height = width * inputs.renderHeight / inputs.renderWidth;

    vec3 camW = normalize(inputs.cameraOrigin.xyz - inputs.cameraTarget.xyz);
    vec3 camU = normalize(cross(inputs.cameraUp.xyz, camW));
    vec3 camV = normalize(cross(camW, camU));

    vec3 horizontal = camU * width;
    vec3 vertical = camV * height;

    vec3 screenStart = inputs.cameraOrigin.xyz - (0.5 * horizontal) + (0.5 * vertical) - focusDistance * camW;

    vec3 pixel = vec3(0, 0, 0);

    for (uint i = 0; i < samples; ++i)
    {
        float sceneX = float(x + random()) / inputs.renderWidth;
        float sceneY = float(y + random()) / inputs.renderHeight;

        vec3 rayOrigin = inputs.cameraOrigin.xyz + randomFocusRing(0.025);

        Ray ray = {
            rayOrigin,
            screenStart + horizontal * sceneX - vertical * sceneY - rayOrigin,
            random(380, 780)
        };

        pixel += inputs.weight
               * trace(ray)
               * vec3(colorSpaceX(ray.wavelength),
                      colorSpaceY(ray.wavelength),
                      colorSpaceZ(ray.wavelength));
    }

    results.pixels[x + inputs.renderWidth * y] += vec4(pixel, 0);
}
