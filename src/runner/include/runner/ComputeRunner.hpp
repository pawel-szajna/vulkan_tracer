#pragma once

#include <io_types.hpp>
#include <types/Basic.hpp>

#include <atomic>
#include <functional>
#include <map>
#include <queue>
#include <string_view>
#include <vector>

namespace vrt::runner
{
class VulkanCompute;

class ComputeRunner
{
public:
    struct ChunkProgress
    {
        u32 x;
        u32 y;
        u32 timePerSample;
        u32 remainingSamples;
    };

    using ChunkProgressData = std::vector<ChunkProgress>;
    using DoneCallback = std::function<void(ComputeRunner&)>;

    ComputeRunner(VulkanCompute& vulkan, InputData scene, std::string_view name, i32 timeTarget);
    void onDone(DoneCallback&& callback);
    void execute(u32 iterations);
    bool done() const;
    void abort();

    const std::vector<float>& results();
    void obtain(ChunkProgressData& progress, std::vector<float>& data);

private:

    VulkanCompute& vulkan;
    InputData scene;

    std::vector<float> data;
    std::string_view name;
    i32 timeTarget;

    std::atomic<bool> running;

    ChunkProgressData chunkProgress;
    std::queue<DoneCallback> doneCallbacks;
};
}
