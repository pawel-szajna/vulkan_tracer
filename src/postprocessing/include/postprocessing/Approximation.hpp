#pragma once

#include <types/Basic.hpp>
#include <postprocessing/Task.hpp>

#include <memory>
#include <queue>
#include <vector>

namespace vrt::postprocessing
{
class Approximation : public Task
{
    struct Threshold
    {
        u32 samples;
        u32 radius;
    };

    struct Sector
    {
        u32 x;
        u32 y;
        u32 radius;
    };

public:

    using Samples = u32;
    using Radius = u32;

    Approximation(std::vector<u32>& pixels, u32 width, u32 height);
    static std::unique_ptr<Approximation> createDefault(std::vector<u32>& pixels, u32 width, u32 height);

    void addThreshold(Samples samples, Radius radius);
    void addSector(u32 x, u32 y, u32 samples);

    void start() override;
    bool done() override;
    Work get() override;

private:

    void processSector();

    std::vector<u32>& pixels;

    std::vector<Threshold> thresholds{};
    std::queue<Sector> sectors{};

    u32 width;
    u32 height;

    u32 remaining;
};
}
