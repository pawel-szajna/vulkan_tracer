#include "ComputeRunner.hpp"

#include "VulkanCompute.hpp"

#include <postprocessing/Executor.hpp>
#include <postprocessing/Variance.hpp>
#include <utils/Helpers.hpp>

#include <indicators/indicators.hpp>
#include <mutex>
#include <numeric>
#include <optional>
#include <spdlog/spdlog.h>

namespace
{
std::mutex mutex{};
}

namespace vrt::runner
{
ComputeRunner::ComputeRunner(VulkanCompute& vulkan,
                             InputData scene,
                             std::string_view name,
                             i32 timeTarget,
                             bool showProgress)
    : vulkan{vulkan}
    , scene{scene}
    , name{name}
    , timeTarget{timeTarget}
    , showProgress{showProgress}
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

void ComputeRunner::onDone(ComputeRunner::DoneCallback&& callback)
{
    if (running)
    {
        SPDLOG_ERROR("Cannot register more callbacks once the task has been launched!");
        return;
    }

    doneCallbacks.emplace(callback);
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

    std::vector<u32> renderOrder{};
    renderOrder.resize(chunksHorizontal * chunksVertical);
    std::iota(renderOrder.begin(), renderOrder.end(), 0u);

    chunkProgress.clear();
    chunkProgress.reserve(chunksHorizontal * chunksVertical);

    for (u32 j = 0; j < chunksVertical; ++j)
    {
        for (u32 i = 0; i < chunksHorizontal; ++i)
        {
            chunkProgress.push_back(ChunkProgress{i, j, targetTime / 2u, iterations});
        }
    }

    u32 total = chunksHorizontal * chunksVertical * iterations;
    u32 remaining = chunksHorizontal * chunksVertical * iterations;

    float invIterations = 1.f / float(iterations);

    std::optional<indicators::BlockProgressBar> progressBar{};
    if (showProgress)
    {
        progressBar.emplace(indicators::option::BarWidth{40},
                            indicators::option::PrefixText{fmt::format("Rendering {} ", name)},
                            indicators::option::ShowElapsedTime{true},
                            indicators::option::ShowRemainingTime{true});
        indicators::show_console_cursor(false);
    }

    postprocessing::Executor postprocess{};

    while (remaining > 0 and running)
    {
        std::sort(renderOrder.begin(),
                  renderOrder.end(),
                  [&p = this->chunkProgress](u32 lhs, u32 rhs)
                  {
                      return p[lhs].timePerSample < p[rhs].timePerSample;
                  });

        for (auto chunkId : renderOrder)
        {
            if (not running)
            {
                break;
            }

            auto& chunk = chunkProgress[chunkId];

            if (chunk.remainingSamples == 0)
            {
                continue;
            }

            u32 samples = 1;
            if (chunk.timePerSample > 0)
            {
                samples = std::min(std::max(1u, targetTime / chunk.timePerSample), chunk.remainingSamples);
            }

            SPDLOG_DEBUG("Rendering chunk {{{}, {}}}. Last average time per sample: {} ms, remaining samples: {}, queued samples: {}",
                         chunk.x, chunk.y, chunk.timePerSample / 1000.f, chunk.remainingSamples, samples);

            changeRandomSeed(scene);
            scene.samplesPerShader = samples;
            scene.offsetX = chunk.x * chunkSize;
            scene.offsetY = chunk.y * chunkSize;
            scene.weight = invIterations;

            std::scoped_lock lock{mutex};
            vulkan.upload(scene);
            u64 execTime = vulkan.execute();
            chunk.remainingSamples -= samples;
            chunk.timePerSample = execTime / samples;
            remaining -= samples;

            if (showProgress)
            {
                progressBar->set_progress(100.f * (total - remaining) / total);
            }
        }
    }

    while (not doneCallbacks.empty())
    {
        doneCallbacks.front()(*this);
        doneCallbacks.pop();
    }

    if (showProgress)
    {
        indicators::show_console_cursor(true);
    }

    running = false;
}

const std::vector<float>& ComputeRunner::results()
{
    vulkan.download(reinterpret_cast<u8*>(data.data()));
    return data;
}

bool ComputeRunner::done() const
{
    return not running;
}

void ComputeRunner::obtain(ChunkProgressData& progress, std::vector<float>& pixels)
{
    std::scoped_lock lock{mutex};
    results();
    progress.assign(chunkProgress.begin(), chunkProgress.end());
    pixels.assign(data.begin(), data.end());
}
}
