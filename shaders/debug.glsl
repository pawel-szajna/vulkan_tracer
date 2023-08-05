const uint debugScale = 5;

void debug(uint x, uint y)
{
    if (y < debugScale * 5)
    {
        switch ((y / debugScale) % 5)
        {
            case 0:
            float k = float(1 + ((x / debugScale) % 4)) / 4;
            results.pixels[x + inputs.width * y] = vec4(0, 0, k, 0);
            break;
            case 1:
            uint n = (x / debugScale) % shapesLimit;
            float m = float(inputs.shapes[n]) / float(shapesLimit);
            results.pixels[x + inputs.width * y] = vec4(m, 0, 0, 0);
            break;
            case 2:
            uint q = (x / debugScale) % vectorsLimit;
            vec4 w = inputs.vectors[q];
            results.pixels[x + inputs.width * y] = w;
            break;
            case 3:
            uint o = (x / debugScale) % scalarsLimit;
            float p = inputs.scalars[o];
            results.pixels[x + inputs.width * y] = vec4(0, p, 0, 0);
            break;
        }
    }
}
