float colorIntensity(float wavelength, vec3 spectralRange)
{
    if (wavelength >= spectralRange.x && wavelength <= spectralRange.y)
    {
        return 1;
    }

    if (wavelength >= spectralRange.x - spectralRange.z && wavelength < spectralRange.x)
    {
        return (spectralRange.x - wavelength) / spectralRange.z;
    }

    if (wavelength > spectralRange.y && wavelength <= spectralRange.y + spectralRange.z)
    {
        return (spectralRange.y + spectralRange.z - wavelength) / spectralRange.z;
    }

    return 0;
}
