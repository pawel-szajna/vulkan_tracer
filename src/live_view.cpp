#include "live_view.hpp"

#include "helpers.hpp"
#include "runner.hpp"
#include "scene.hpp"

#include <chrono>
#include <spdlog/spdlog.h>
#include <thread>

#define LiveViewWindowConstructor LiveViewWindow(int width, int height)
#define LiveViewWindowUpdate bool update(const std::vector<u32>& pixels)

#if defined(HAS_LIVE_VIEW)
#include <SDL.h>
#include <SDL_events.h>

class LiveViewWindow
{
public:

    LiveViewWindowConstructor
        : width{width}
    {
        SPDLOG_DEBUG("Creating live view window {}x{}", width, height);
        SDL_Init(SDL_INIT_EVERYTHING);
        window = SDL_CreateWindow("Live preview", 0, 0, width, height, 0);
        renderer = SDL_CreateRenderer(window, 0, 0);
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    }

    ~LiveViewWindow()
    {
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    LiveViewWindowUpdate
    {
        static SDL_Event event{};
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                return false;
            }
        }

        SDL_UpdateTexture(texture, nullptr, pixels.data(), width * static_cast<int>(sizeof(u32)));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        return true;
    }

private:

    int width;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};
#else

class LiveViewWindow
{
public:

    LiveViewWindowConstructor
    {
        SPDLOG_ERROR("Application has not been built with live view support");
        throw std::runtime_error("Unsupported");
    }

    LiveViewWindowUpdate
    {
        return false;
    }
};

#endif

LiveView::LiveView(const SceneBuilder& scene, ComputeRunner& runner)
    : scene{scene}
    , runner{runner}
    , window{std::make_unique<LiveViewWindow>(scene.getResolutionWidth(),
                                              scene.getResolutionHeight())}
{}

LiveView::~LiveView()
{
    runner.abort();
}

void LiveView::start()
{
    std::vector<u32> pixels;
    auto width = scene.getResolutionWidth();
    auto height = scene.getResolutionHeight();
    pixels.reserve(width * height);

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
                auto [r, g, b] = xyzToRgb(chunkWeight * data[(x + y * width) * 4],
                                          chunkWeight * data[(x + y * width) * 4 + 1],
                                          chunkWeight * data[(x + y * width) * 4 + 2]);
                pixels[x + (height - y - 1) * width]
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
