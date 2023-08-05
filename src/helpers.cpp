#include "helpers.hpp"

#include <fstream>
#include <spdlog/spdlog.h>

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
    inputs.randomSeed =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
}

void save(const OutputData& data, std::string_view filename)
{
    SPDLOG_INFO("Saving image to {}", filename);
    TIMER_START;
    std::ofstream picture{filename};
    picture << "P3\n";
    picture << renderWidth << ' ' << renderHeight << " 255\n";

    for (int y = renderHeight - 1; y >= 0; --y)
    {
        for (int x = 0; x < renderWidth; ++x)
        {
            int r = static_cast<int>(256 * std::clamp(data.pixels[(x + y * 640) * 4], 0.f, 0.999f));
            int g = static_cast<int>(256 * std::clamp(data.pixels[(x + y * 640) * 4 + 1], 0.f, 0.999f));
            int b = static_cast<int>(256 * std::clamp(data.pixels[(x + y * 640) * 4 + 2], 0.f, 0.999f));
            picture << r << ' ' << g << ' ' << b << '\n';
        }
    }

    TIMER_END("File writing");
}
