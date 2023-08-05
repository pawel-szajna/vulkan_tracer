#include "basic_types.hpp"
#include "constants.hpp"
#include "helpers.hpp"
#include "io_types.hpp"
#include "scene.hpp"
#include "vulkan_compute.hpp"

#include <spdlog/spdlog.h>
#include <string_view>

int main()
{
    SPDLOG_INFO("GPU Raytracer startup");
    SPDLOG_INFO("    Render width: {}", renderWidth);
    SPDLOG_INFO("    Render height: {}", renderHeight);

    TIMER_START;

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
        scene.addSphere(vec3{0.75, 0.5, 1}, 0.55);

        vc.upload(scene.build());
        vc.execute();

        auto data = vc.download<OutputData>();
        save(data, "output.ppm");
    }
    catch (const std::exception& e)
    {
        SPDLOG_CRITICAL("Execution failed: {}", e.what());
    }

    TIMER_END("Raytracer execution");

    return 0;
}
