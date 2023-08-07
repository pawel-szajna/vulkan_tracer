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

    SPDLOG_INFO("Execution targets {} total iterations, yielding {}M total virtual photons",
                iterations,
                (u64)iterations * scene.renderWidth * scene.renderHeight / 1'000'000);

    std::vector<float> batchResult{};
    batchResult.resize(scene.renderWidth * scene.renderHeight * 4);

    constexpr static u32 targetTimeMs = 150;
    constexpr static u32 chunkSize = 64;

    u32 chunksHorizontal = scene.renderWidth / chunkSize;
    u32 chunksVertical = scene.renderHeight / chunkSize;

    std::map<std::pair<u32, u32>, std::pair<u32, u32>> chunkProgress{};

    for (u32 i = 0; i < chunksHorizontal; ++i)
    {
        for (u32 j = 0; j < chunksVertical; ++j)
        {
            chunkProgress[{i, j}] = {iterations, targetTimeMs};
        }
    }

    u32 total = chunksHorizontal * chunksVertical * iterations;
    u32 remaining = chunksHorizontal * chunksVertical * iterations;

    float invIterations = 1.f / float(iterations);

    while (remaining > 0)
    {
        for (u32 i = 0; i < chunksHorizontal; ++i)
        {
            for (u32 j = 0; j < chunksVertical; ++j)
            {
                if (chunkProgress[{i, j}].first == 0)
                {
                    continue;
                }

                u32 samples = std::min(std::max(1u, targetTimeMs / chunkProgress[{i, j}].second), chunkProgress[{i, j}].first);
                SPDLOG_DEBUG("Rendering chunk {{{}, {}}}. Last time: {} ms, samples: {}, remaining: {}",
                             i, j, chunkProgress[{i, j}].second, samples, chunkProgress[{i, j}].first);

                changeRandomSeed(scene);
                scene.samplesPerShader = samples;
                scene.offsetX = i * chunkSize;
                scene.offsetY = j * chunkSize;
                scene.weight = invIterations;

                vulkan.upload(scene);
                u64 execTime = vulkan.execute() / 1000;

                chunkProgress[{i, j}].first -= samples;
                chunkProgress[{i, j}].second = execTime / samples;
                remaining -= samples;

                SPDLOG_INFO("Rendering progress: {}%", 100 * (total - remaining) / total);
            }
        }
    }

    vulkan.download(reinterpret_cast<u8*>(data.data()));
}

std::vector<float> ComputeRunner::results()
{
    return data;
}
