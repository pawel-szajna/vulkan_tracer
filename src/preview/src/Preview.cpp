#include "Preview.hpp"

#include "Window.hpp"

#include <runner/ComputeRunner.hpp>
#include <scene/Scene.hpp>
#include <utils/Helpers.hpp>

#include <thread>

namespace vrt::preview
{
Preview::Preview(const scene::Scene& scene, runner::ComputeRunner& runner, float scale)
    : scene{scene}
    , runner{runner}
    , window{std::make_unique<Window>(scene.getResolutionWidth(),
                                      scene.getResolutionHeight(),
                                      scale)}
{}

Preview::~Preview()
{
    runner.abort();
}

void Preview::start()
{
    std::vector<u32> pixels;
    std::vector<float> data;
    runner::ComputeRunner::ChunkProgressData progress;

    auto width = scene.getResolutionWidth();
    auto height = scene.getResolutionHeight();
    auto iterations = scene.getTargetIterations();

    pixels.resize(width * height);
    data.reserve(width * height * 4);

    do
    {
        runner.obtain(progress, data);

        if (progress.empty())
        {
            continue;
        }

        for (int y = 0; y < height; ++y)
        {
            int segmentY = y / 64;
            for (int x = 0; x < width; ++x)
            {
                int segmentX = x / 64;
                auto& chunkInfo = progress[segmentX + segmentY * width / 64];
                auto chunkDoneIterations = std::max(1u, iterations - chunkInfo.remainingSamples);
                float chunkWeight = float(iterations) / float(chunkDoneIterations);
                auto [r, g, b] = xyzToRgb(chunkWeight * data[(x + y * width) * 4],
                                          chunkWeight * data[(x + y * width) * 4 + 1],
                                          chunkWeight * data[(x + y * width) * 4 + 2]);
                pixels[x + y * width]
                    = exportColor(r) << 24
                    | exportColor(g) << 16
                    | exportColor(b) << 8
                    | 0xff;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds{200});
    }
    while (window->update(pixels));
    runner.abort();
}
}
