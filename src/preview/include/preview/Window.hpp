#pragma once

#include <types/Basic.hpp>

#include <vector>

#if defined(HAS_LIVE_VIEW)
#include <SDL.h>
#include <SDL_events.h>
#endif

namespace vrt::preview
{
#if defined(HAS_LIVE_VIEW)

class Window
{
public:

    Window(int width, int height, float scale);
    ~Window();

    bool update(const std::vector<u32>& pixels);

private:

    int width;
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};

#else

class Window
{
public:

     Window(int width, int height, float scale);
     bool update(const std::vector<u32>& pixels);

};

#endif
}
