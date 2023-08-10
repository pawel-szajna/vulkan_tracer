#include "GaussFilter.hpp"

#include <types/Image.hpp>
#include <types/RGBPixel.hpp>

#include <chrono>
#include <mutex>
#include <numeric>
#include <thread>

namespace
{
std::mutex mutex{};

using vrt::types::Image;
using vrt::types::RGBPixel;

using Window = std::vector<RGBPixel>;

constexpr u32 weight = 273u;
const std::vector kernel {
    1u,  4u,  7u,  4u,  1u,
    4u, 16u, 26u, 16u,  4u,
    7u, 26u, 41u, 26u,  7u,
    4u, 16u, 26u, 16u,  4u,
    1u,  4u,  7u,  4u,  1u
};

inline Window createWindow(const Image& image, i32 x, i32 y)
{
    // clang-format off
    return Window {
        image[x - 2, y - 2], image[x - 1, y - 2], image[    x, y - 2], image[x + 1, y - 2], image[x + 2, y - 2],
        image[x - 2, y - 1], image[x - 1, y - 1], image[    x, y - 1], image[x + 1, y - 1], image[x + 2, y - 1],
        image[x - 2,     y], image[x - 1,     y], image[    x,     y], image[x + 1,     y], image[x + 2,     y],
        image[x - 2, y + 1], image[x - 1, y + 1], image[    x, y + 1], image[x + 1, y + 1], image[x + 2, y + 1],
        image[x - 2, y + 2], image[x - 1, y + 2], image[    x, y + 2], image[x + 1, y + 2], image[x + 2, y + 2],
    };
    // clang-format on
}

inline void moveWindowRight(const Image& image, Window& window, i32 newx, i32 newy)
{
    for (i32 y = 0; y < 5; ++y)
    {
        for (i32 x = 0; x < 4; ++x)
        {
            window[x + 5 * y] = window[x + 1 + 5 * y];
        }
        window[4 + 5 * y] = image[newx + 2, newy + y - 2];
    }
}
}

namespace vrt::postprocessing
{
GaussFilter::GaussFilter(std::vector<u32>& pixels, u32 width, u32 height)
    : pixels{pixels}
    , width{width}
    , height{height}
{}

void GaussFilter::start()
{
    next = 0;
    remaining = height;
}

Task::Work GaussFilter::get()
{
    return [&]{ processRow(); };
}

bool GaussFilter::done()
{
    std::lock_guard lock{mutex};
    return remaining == 0;
}

void GaussFilter::processRow()
{
    u32 row = 0;
    {
        std::lock_guard lock{mutex};
        if (remaining == 0 or next >= height)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return;
        }
        row = next++;
    }

    std::vector<u32> result;
    result.reserve(width);

    Image img{pixels, width, height};

    Window multiplied;
    multiplied.reserve(5 * 5);

    auto window = createWindow(img, 0, row);

    for (i32 x = 0; x < width; ++x)
    {
        multiplied.clear();
        std::transform(window.begin(),
                       window.end(),
                       kernel.begin(),
                       std::back_inserter(multiplied),
                       std::multiplies<>());
        result.push_back(u32(std::accumulate(multiplied.begin(),
                                             multiplied.end(),
                                             RGBPixel{0, 0, 0}) / 273));
        moveWindowRight(img, window, x + 1, row);
    }

    std::copy(result.begin(), result.end(), pixels.begin() + row * width);

    std::lock_guard lock{mutex};
    --remaining;
}
}
