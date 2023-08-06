#include materials/material

float Light_shine(float wavelength, int material)
{
    DataUsage usage = materialsDataUsage(material);
    return inputs.scalars[usage.scalars] * colorIntensity(wavelength, inputs.vectors[usage.vectors].xyz);
}
