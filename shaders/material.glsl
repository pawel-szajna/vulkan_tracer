struct DataUsage
{
    uint vectors;
    uint scalars;
    uint integers;
};

DataUsage materialsDataUsage(uint limit)
{
    DataUsage results = DataUsage(0, 0, 0);

    for (uint i = 0; i < limit; ++i)
    {
        results.vectors++;
    }

    return results;
}
