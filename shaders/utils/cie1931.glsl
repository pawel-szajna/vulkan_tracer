#pragma once
#include "../glsl_builtin.h"

float cie1931(float wavelength, float mi, float sigma1inv, float sigma2inv)
{
    if (wavelength < mi)
    {
        return exp(-0.5 * (wavelength - mi) * (wavelength - mi) * sigma1inv);
    }

    return exp(-0.5 * (wavelength - mi) * (wavelength - mi) * sigma2inv);
}

float colorSpaceX(float wavelength)
{
    return 1.056 * cie1931(wavelength, 599.8, 0.00069618006,  0.001040582726)
         + 0.362 * cie1931(wavelength, 442.0, 0.00390625,     0.001402740956)
         - 0.065 * cie1931(wavelength, 501.1, 0.002402921953, 0.001456791562);
}

float colorSpaceY(float wavelength)
{
    return 0.821 * cie1931(wavelength, 568.8, 0.0004546260473, 0.0006096631611)
         + 0.286 * cie1931(wavelength, 530.9, 0.003763784862,  0.001033901635);
}

float colorSpaceZ(float wavelength)
{
    return 1.217 * cie1931(wavelength, 437.0, 0.007181844298, 0.0007716049383)
         + 0.681 * cie1931(wavelength, 459.0, 0.001479289941, 0.00525099769);
}
