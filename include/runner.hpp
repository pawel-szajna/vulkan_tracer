#pragma once

#include "io_types.hpp"

#include <atomic>
#include <string_view>
#include <vector>

class VulkanCompute;

class ComputeRunner
{
public:

    ComputeRunner(VulkanCompute& vulkan, InputData scene, std::string_view name);
    void execute(u32 iterations);
    void abort();
    std::vector<float> results();

private:

    VulkanCompute& vulkan;
    InputData scene;

    std::vector<float> data;
    std::string_view name;

    std::atomic<bool> running;
};
