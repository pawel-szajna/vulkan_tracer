#include "runner.hpp"

#include "helpers.hpp"
#include "vulkan_compute.hpp"

#include <indicators/indicators.hpp>
#include <stdexcept>
#include <spdlog/spdlog.h>

ComputeRunner::ComputeRunner(VulkanCompute& vulkan, InputData scene, std::string_view name, i32 timeTarget)
    : vulkan{vulkan}
    , scene{scene}
    , name{name}
    , timeTarget{timeTarget}
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

    SPDLOG_INFO("Execution targets {} total iterations, yielding {}M total virtual photons",
                iterations,
                (u64)iterations * scene.renderWidth * scene.renderHeight / 1'000'000);

    std::vector<float> batchResult{};
    batchResult.resize(scene.renderWidth * scene.renderHeight * 4);

    constexpr static u32 chunkSize = 64;
    auto targetTime = timeTarget * 1000;

    u32 chunksHorizontal = scene.renderWidth / chunkSize;
    u32 chunksVertical = scene.renderHeight / chunkSize;

    chunkProgress.clear();

    for (u32 i = 0; i < chunksHorizontal; ++i)
    {
        for (u32 j = 0; j < chunksVertical; ++j)
        {
            chunkProgress[{i, j}] = {iterations, targetTime / 4};
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

    while (remaining > 0)
    {
        for (u32 i = 0; i < chunksHorizontal; ++i)
        {
            for (u32 j = 0; j < chunksVertical and running; ++j)
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

std::pair<std::vector<float>*, ComputeRunner::ChunkProgressMap*> ComputeRunner::results()
{
    vulkan.download(reinterpret_cast<u8*>(data.data()));
    return std::make_pair(&data, &chunkProgress);
}
