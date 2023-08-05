#include "helpers.hpp"

std::ostream& operator<<(std::ostream& os, const ioVec& v)
{
    return os << '[' << v.x << ',' << v.y << ',' << v.z << ']';
}

std::ostream& operator<<(std::ostream& os, const InputData& inputs)
{
    os << "Input data" << std::endl
       << "    Random seed: " << inputs.randomSeed << std::endl
       << "    Shapes count: " << inputs.shapesCount << std::endl
       << "    Shapes: " << Printable<u32>{inputs.shapes, shapesLimit} << std::endl
       << "    Vectors: " << Printable<float>{inputs.vectors, vectorsLimit, 4} << std::endl
       << "    Scalars: " << Printable<float>{inputs.scalars, scalarsLimit} << std::endl;
    return os;
}

void seedFromClock(InputData& inputs)
{
    inputs.randomSeed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
}
