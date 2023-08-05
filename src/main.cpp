#include "basic_types.hpp"
#include "constants.hpp"
#include "io_types.hpp"
#include "scene.hpp"
#include "vulkan_compute.hpp"

#include <fstream>
#include <spdlog/spdlog.h>
#include <string_view>

void save(const OutputData& data, std::string_view filename)
{
    SPDLOG_INFO("Saving image to {}", filename);
    std::ofstream picture{filename};
    picture << "P3\n";
    picture << renderWidth << ' ' << renderHeight << " 255\n";

    for (int y = renderHeight - 1; y >= 0; --y)
    {
        for (int x = 0; x < renderWidth; ++x)
        {
            int r = static_cast<int>(256 * std::clamp(data.pixels[(x + y * 640) * 4], 0.f, 0.999f));
            int g = static_cast<int>(256 * std::clamp(data.pixels[(x + y * 640) * 4 + 1], 0.f, 0.999f));
            int b = static_cast<int>(256 * std::clamp(data.pixels[(x + y * 640) * 4 + 2], 0.f, 0.999f));
            picture << r << ' ' << g << ' ' << b << '\n';
        }
    }
}

int main()
{
    SPDLOG_INFO("GPU Raytracer startup");
    SPDLOG_INFO("    Render width: {}", renderWidth);
    SPDLOG_INFO("    Render height: {}", renderHeight);

    if (SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG)
    {
        spdlog::set_level(spdlog::level::debug);
    }

    try
    {
        VulkanCompute vc{inputSize, outputSize, "main.spv", renderWidth, renderHeight, 1};

        SceneBuilder scene{};
        scene.setResolution(renderWidth, renderHeight);
        scene.setSamplesPerShaderPass(samplesPerShaderPass);
        scene.addSphere(vec3{0, 0, -2}, 0.5);

        vc.upload(scene.build());
        vc.execute();

        auto data = vc.download<OutputData>();
        save(data, "output.ppm");
    }
    catch (const std::exception& e)
    {
        SPDLOG_CRITICAL("Execution failed: {}", e.what());
    }

    return 0;
}
