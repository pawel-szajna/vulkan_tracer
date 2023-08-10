#pragma once

#include <postprocessing/Task.hpp>
#include <types/Basic.hpp>

#include <vector>

namespace vrt::postprocessing
{
class GaussFilter : public Task
{
public:

    GaussFilter(std::vector<u32>& pixels, u32 width, u32 height);

    void start() override;
    bool done() override;
    Work get() override;

private:

    void processRow();

    std::vector<u32>& pixels;

    u32 width;
    u32 height;

    u32 remaining;
    u32 next;
};
}
