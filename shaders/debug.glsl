void debug(uint x, uint y)
{
    if (y < 20 * 5)
    {
        switch ((y / 20) % 5)
        {
            case 0:
            float k = float(1 + ((x / 20) % 4)) / 4;
            results.pixels[x + inputs.width * y] = vec4(0, 0, k, 0);
            break;
            case 1:
            uint n = (x / 20) % shapesLimit;
            float m = float(inputs.shapes[n]) / float(shapesLimit);
            results.pixels[x + inputs.width * y] = vec4(m, 0, 0, 0);
            break;
            case 2:
            uint q = (x / 20) % vectorsLimit;
            vec4 w = inputs.vectors[q];
            results.pixels[x + inputs.width * y] = w;
            break;
            case 3:
            uint o = (x / 20) % scalarsLimit;
            float p = inputs.scalars[o];
            results.pixels[x + inputs.width * y] = vec4(0, p, 0, 0);
            break;
        }
    }
}
