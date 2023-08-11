#include "Approximation.hpp"

#include <algorithm>
#include <mutex>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace
{
std::mutex mutex{};
}

namespace vrt::postprocessing
{
std::unique_ptr<Approximation> Approximation::createDefault(std::vector<u32>& pixels, u32 width, u32 height)
{
    auto approximation = std::make_unique<Approximation>(pixels, width, height);
    approximation->addThreshold(Samples{4}, Radius{64});
    approximation->addThreshold(Samples{20}, Radius{32});
    approximation->addThreshold(Samples{50}, Radius{16});
    approximation->addThreshold(Samples{100}, Radius{8});
    approximation->addThreshold(Samples{160}, Radius{4});
    approximation->addThreshold(Samples{250}, Radius{2});
    return approximation;
}

Approximation::Approximation(std::vector<u32>& pixels, u32 width, u32 height)
    : pixels{pixels}
    , width{width}
    , height{height}
{}

void Approximation::addThreshold(u32 samples, u32 radius)
{
    if (not sectors.empty())
    {
        throw std::logic_error("Cannot modify thresholds after sectors have been added");
    }

    if (64 % radius != 0)
    {
        throw std::invalid_argument("Block size (64) must be divisible by threshold radius");
    }

    thresholds.emplace_back(Threshold{samples, radius});
}

void Approximation::addSector(u32 x, u32 y, u32 samples)
{
    if (thresholds.empty())
    {
        throw std::logic_error("Sectors should be added to Approximation task only after adding all thresholds");
    }

    if (sectors.empty())
    {
        std::sort(thresholds.begin(),
                  thresholds.end(),
                  [](const auto& lhs, const auto& rhs) { return lhs.samples < rhs.samples; });
    }

    for (const auto& threshold : thresholds)
    {
        if (samples < threshold.samples)
        {
            sectors.emplace(Sector{x, y, threshold.radius});
        }
    }
}

bool Approximation::done()
{
    std::lock_guard lock{mutex};
    return remaining == 0;
}

void Approximation::start()
{
    remaining = sectors.size();
}

Task::Work Approximation::get()
{
    return [&]{ processSector(); };
}

void Approximation::processSector()
{
    Sector sector;
    {
        std::lock_guard lock{mutex};
        if (sectors.empty())
        {
            return;
        }
        sector = sectors.front();
        sectors.pop();
    }

    u32 segmentX = sector.x * 64;
    u32 segmentY = sector.y * 64;

    u32 weight = sector.radius * sector.radius;
    u32 blockSize = 64 / sector.radius;

    for (u32 j = 0; j < blockSize; ++j)
    {
        for (u32 i = 0; i < blockSize; ++i)
        {
            u32 blockX = i * sector.radius;
            u32 blockY = j * sector.radius;

            u32 red = 0;
            u32 green = 0;
            u32 blue = 0;

            for (u32 n = 0; n < sector.radius; ++n)
            {
                for (u32 m = 0; m < sector.radius; ++m)
                {
                    auto offset = (segmentX + blockX + m) + (segmentY + blockY + n) * width;
                    u32 value = pixels[offset];
                    red   += (value & 0xff000000) >> 24;
                    green += (value & 0x00ff0000) >> 16;
                    blue  += (value & 0x0000ff00) >> 8;
                }
            }

            red /= weight;
            green /= weight;
            blue /= weight;

            for (u32 n = 0; n < sector.radius; ++n)
            {
                for (u32 m = 0; m < sector.radius; ++m)
                {
                    auto offset = (segmentX + blockX + m) + (segmentY + blockY + n) * width;
                    pixels[offset] = std::clamp(red, 0u, 255u) << 24
                                   | std::clamp(green, 0u, 255u) << 16
                                   | std::clamp(blue, 0u, 255u) << 8
                                   | 0xff;
                }
            }
        }
    }

    std::lock_guard lock{mutex};
    remaining -= 1;
}
}
