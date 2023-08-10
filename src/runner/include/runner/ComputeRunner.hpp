#pragma once

#include <io_types.hpp>
#include <types/Basic.hpp>

#include <atomic>
#include <map>
#include <string_view>
#include <vector>

class VulkanCompute;

class ComputeRunner
{
public:

    using ChunkProgressMap = std::map<std::pair<u32, u32>, std::pair<u32, u32>>;

    ComputeRunner(VulkanCompute& vulkan, InputData scene, std::string_view name, i32 timeTarget);
    void execute(u32 iterations);
    void abort();
    std::pair<std::vector<float>*, ComputeRunner::ChunkProgressMap*> results();

private:

    VulkanCompute& vulkan;
    InputData scene;

    std::vector<float> data;
    std::string_view name;
    i32 timeTarget;

    std::atomic<bool> running;

    ChunkProgressMap chunkProgress;
};
