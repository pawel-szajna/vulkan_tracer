#include "Preview.hpp"

#include "Window.hpp"

#include <runner/ComputeRunner.hpp>
#include <scene/scene.hpp>
#include <utils/Helpers.hpp>

#include <thread>

namespace vrt::preview
{
Preview::Preview(const SceneBuilder& scene, ComputeRunner& runner, float scale)
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
    auto width = scene.getResolutionWidth();
    auto height = scene.getResolutionHeight();
    pixels.resize(width * height);

    do
    {
        auto [data, chunkProgress] = runner.results();
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int segmentX = x / 64;
                int segmentY = y / 64;
                auto currentChunkIterations = chunkProgress->operator[]({segmentX, segmentY}).first;
                auto chunkDoneIterations = scene.getTargetIterations() - currentChunkIterations + 1;
                float chunkWeight = float(scene.getTargetIterations()) / float(chunkDoneIterations);
                auto [r, g, b] = xyzToRgb(chunkWeight * data->operator[]((x + y * width) * 4),
                                          chunkWeight * data->operator[]((x + y * width) * 4 + 1),
                                          chunkWeight * data->operator[]((x + y * width) * 4 + 2));
                pixels[x + (height - y - 1) * width]
                    = exportColor(r) << 24
                    | exportColor(g) << 16
                    | exportColor(b) << 8
                    | 0xff;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds{250});
    }
    while (window->update(pixels));
    runner.abort();
}
}
