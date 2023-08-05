#include "basic_types.hpp"
#include "helpers.hpp"
#include "io_types.hpp"
#include "scene_reader.hpp"
#include "vulkan_compute.hpp"

#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>
#include <string_view>

int main(int argc, char** argv)
{
    SPDLOG_INFO("GPU Raytracer startup");
    TIMER_START;

    if (SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG)
    {
        spdlog::set_level(spdlog::level::debug);
    }

    argparse::ArgumentParser args("vulkan_tracer");
    args.add_argument("scene")
        .help("YML file with scene definition");

    try
    {
        args.parse_args(argc, argv);
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << args;
        return 1;
    }

    try
    {
        auto scene = SceneReader::read(args.get("scene"));

        auto width  = scene.getResolutionWidth();
        auto height = scene.getResolutionHeight();

        VulkanCompute vc{inputSize, width * height * sizeof(float) * 4, "main.spv", width, height, 1};
        vc.upload(scene.build());

        u64 limit = 1e9;
        vc.execute(limit);

        std::vector<float> data(width * height, 0.f);
        vc.download(reinterpret_cast<u8*>(data.data()));
        save(data, width, height, "output.ppm");
    }
    catch (const std::exception& e)
    {
        SPDLOG_CRITICAL("Execution failed: {}", e.what());
    }

    TIMER_END("Raytracer execution");

    return 0;
}
