#include "runner.hpp"

#include "helpers.hpp"
#include "vulkan_compute.hpp"

#include <indicators/indicators.hpp>
#include <stdexcept>
#include <spdlog/spdlog.h>

ComputeRunner::ComputeRunner(VulkanCompute& vulkan, InputData scene, std::string_view name)
    : vulkan{vulkan}
    , scene{scene}
    , name{name}
{
    data.resize(scene.renderWidth * scene.renderHeight * 4);
    auto slash = name.rfind('/');
    if (slash != std::string_view::npos)
    {
        this->name.remove_prefix(slash + 1);
    }
}

void ComputeRunner::abort()
{
    running = false;
}

void ComputeRunner::execute(u32 iterations)
{
    running = true;

    if (iterations % scene.samplesPerShader > 0)
    {
        throw std::invalid_argument("Requested iterations count is not a multiple of per shader sample count");
    }

    SPDLOG_INFO("Execution targets {} total iterations, yielding {}M total virtual photons",
                iterations,
                (u64)iterations * scene.renderWidth * scene.renderHeight / 1'000'000);

    std::vector<float> batchResult{};
    batchResult.resize(scene.renderWidth * scene.renderHeight * 4);

    constexpr static u32 targetTime = 200'000;
    constexpr static u32 chunkSize = 64;

    u32 chunksHorizontal = scene.renderWidth / chunkSize;
    u32 chunksVertical = scene.renderHeight / chunkSize;

    std::map<std::pair<u32, u32>, std::pair<u32, u32>> chunkProgress{};

    for (u32 i = 0; i < chunksHorizontal; ++i)
    {
        for (u32 j = 0; j < chunksVertical; ++j)
        {
            chunkProgress[{i, j}] = {iterations, 0};
        }
    }

    u32 total = chunksHorizontal * chunksVertical * iterations;
    u32 remaining = chunksHorizontal * chunksVertical * iterations;

    float invIterations = 1.f / float(iterations);

    indicators::BlockProgressBar progressBar{
        indicators::option::BarWidth{40},
        indicators::option::PrefixText{fmt::format("Rendering {} ", name)},
        indicators::option::ShowElapsedTime{true},
        indicators::option::ShowRemainingTime{true}
    };
    indicators::show_console_cursor(false);

    while (remaining > 0 and running)
    {
        for (u32 i = 0; i < chunksHorizontal; ++i)
        {
            for (u32 j = 0; j < chunksVertical; ++j)
            {
                if (chunkProgress[{i, j}].first == 0)
                {
                    continue;
                }

                u32 samples = 1;
                if (chunkProgress[{i, j}].second > 0)
                {
                    samples = std::min(std::max(1u, targetTime / chunkProgress[{i, j}].second), chunkProgress[{i, j}].first);
                }
                SPDLOG_DEBUG("Rendering chunk {{{}, {}}}. Last average time per sample: {} ms, remaining samples: {}, queued samples: {}, ",
                             i, j, chunkProgress[{i, j}].second / 1000.f, chunkProgress[{i, j}].first, samples);

                changeRandomSeed(scene);
                scene.samplesPerShader = samples;
                scene.offsetX = i * chunkSize;
                scene.offsetY = j * chunkSize;
                scene.weight = invIterations;

                vulkan.upload(scene);
                u64 execTime = vulkan.execute();

                chunkProgress[{i, j}].first -= samples;
                chunkProgress[{i, j}].second = execTime / samples;
                remaining -= samples;

                SPDLOG_INFO("Rendering progress: {}%", 100 * (total - remaining) / total);
                progressBar.set_progress(100.f * (total - remaining) / total);
            }
        }
    }

    indicators::show_console_cursor(true);
}

std::vector<float> ComputeRunner::results()
{
    vulkan.download(reinterpret_cast<u8*>(data.data()));
    return data;
}
