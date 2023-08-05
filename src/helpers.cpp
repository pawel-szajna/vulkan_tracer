#include "helpers.hpp"

#include "timers.hpp"

#include <cmath>
#include <fstream>
#include <random>
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

void changeRandomSeed(InputData& inputs)
{
    static std::mt19937 mt{std::random_device{}()};
    inputs.randomSeed = mt();
}

void save(const std::vector<float>& data, u32 width, u32 height, std::string_view filename)
{
    SPDLOG_INFO("Saving image to {}", filename);
    auto timer = Timers::create("File writing");
    std::ofstream picture{filename};
    picture << "P3\n";
    picture << width << ' ' << height << " 255\n";

    for (int y = height - 1; y >= 0; --y)
    {
        for (int x = 0; x < width; ++x)
        {
            int r = static_cast<int>(256 * std::clamp(std::sqrt(data[(x + y * width) * 4]), 0.f, 0.999f));
            int g = static_cast<int>(256 * std::clamp(std::sqrt(data[(x + y * width) * 4 + 1]), 0.f, 0.999f));
            int b = static_cast<int>(256 * std::clamp(std::sqrt(data[(x + y * width) * 4 + 2]), 0.f, 0.999f));
            picture << r << ' ' << g << ' ' << b << '\n';
        }
    }
}
