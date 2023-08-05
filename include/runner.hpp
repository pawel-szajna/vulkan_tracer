#pragma once

#include "io_types.hpp"

#include <vector>

class VulkanCompute;

class ComputeRunner
{
public:

    ComputeRunner(VulkanCompute& vulkan, InputData scene);
    void execute(u32 iterations);
    std::vector<float> results();

private:

    VulkanCompute& vulkan;
    InputData scene;

    std::vector<float> data;
};
