#include "helpers.hpp"

#include "timers.hpp"

#include <cmath>
#include <fstream>
#include <random>
#include <spdlog/spdlog.h>

unsigned exportColor(float color)
{
    return static_cast<unsigned>(256 * std::clamp(std::sqrt(color), 0.f, 0.999f));
}

std::tuple<float, float, float> xyzToRgb(float x, float y, float z)
{
    constexpr static float n = 3400850.0;

    constexpr static float a11 = 8041697.0 / n;
    constexpr static float a12 = -3049000.0 / n;
    constexpr static float a13 = -1591847.0 / n;
    constexpr static float a21 = -1752003.0 / n;
    constexpr static float a22 = 4851000.0 / n;
    constexpr static float a23 = 301853.0 / n;
    constexpr static float a31 = 17697.0 / n;
    constexpr static float a32 = -49000.0 / n;
    constexpr static float a33 = 3432153.0 / n;

    auto R = (a11 * x + a12 * y + a13 * z);
    auto G = (a21 * x + a22 * y + a23 * z);
    auto B = (a31 * x + a32 * y + a33 * z);

    auto overflowR = std::max(R - 1.f, 0.f);
    auto overflowG = std::max(G - 1.f, 0.f);
    auto overflowB = std::max(B - 1.f, 0.f);

    R += overflowG + overflowB;
    G += overflowR + overflowB;
    B += overflowR + overflowG;

    return {R, G, B};
}

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
    std::ofstream picture{std::string(filename)};
    picture << "P3\n";
    picture << width << ' ' << height << " 255\n";

    for (int y = height - 1; y >= 0; --y)
    {
        for (int x = 0; x < width; ++x)
        {
            auto [r, g, b] = xyzToRgb(data[(x + y * width) * 4],
                                      data[(x + y * width) * 4 + 1],
                                      data[(x + y * width) * 4 + 2]);
            picture << exportColor(r) << ' '
                    << exportColor(g) << ' '
                    << exportColor(b) << '\n';
        }
    }
}
