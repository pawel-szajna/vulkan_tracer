#include "runner.hpp"

#include "helpers.hpp"
#include "vulkan_compute.hpp"

#include <stdexcept>

ComputeRunner::ComputeRunner(VulkanCompute& vulkan, InputData scene)
    : vulkan{vulkan}
    , scene{scene}
{
    data.resize(scene.renderWidth * scene.renderHeight * 4);
}

void ComputeRunner::execute(u32 iterations)
{
    if (iterations % scene.samplesPerShader > 0)
    {
        throw std::invalid_argument("Requested iterations count is not a multiple of per shader sample count");
    }

    auto batches = iterations / scene.samplesPerShader;

    SPDLOG_INFO("Execution planned in {} batches of {} samples per batch, yielding {} iterations",
                batches,
                scene.samplesPerShader,
                iterations);

    std::vector<float> batchResult{};
    batchResult.resize(scene.renderWidth * scene.renderHeight * 4);

    auto weight = 1.f / float(batches);

    for (usize i = 0; i < batches; ++i)
    {
        changeRandomSeed(scene);
        SPDLOG_DEBUG("Starting batch {} of {}, random seed: {}",
                     i + 1, batches, scene.randomSeed);

        vulkan.upload(scene);
        vulkan.execute(1e10);
        vulkan.download(reinterpret_cast<u8*>(batchResult.data()));
        auto timer = Timers::create("Data aggregation");
        for (usize pixel = 0; pixel < batchResult.size(); ++pixel)
        {
            data[pixel] += weight * batchResult[pixel];
        }
    }
}

std::vector<float> ComputeRunner::results()
{
    return data;
}
