#include "Window.hpp"

#include <spdlog/spdlog.h>

namespace vrt::preview
{
#if defined(HAS_LIVE_VIEW)

Window::Window(int width, int height, float scale)
    : width{width}
{
    SPDLOG_DEBUG("Creating live view window {}x{}", width * scale, height * scale);
    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("Live preview", 0, 0, width * scale, height * scale, 0);
    renderer = SDL_CreateRenderer(window, 0, 0);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
}

Window::~Window()
{
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Window::update(const std::vector<u32>& pixels)
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

#else

Window::Window(int, int, float)
{
    SPDLOG_ERROR("Application has not been built with live view support");
    throw std::runtime_error("Unsupported");
}

bool Window::update(const std::vector<u32>&)
{
    return false;
}

#endif
}
