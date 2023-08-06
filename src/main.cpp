#include "helpers.hpp"
#include "io_types.hpp"
#include "runner.hpp"
#include "scene_reader.hpp"
#include "timers.hpp"
#include "vulkan_compute.hpp"

#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>

int main(int argc, char** argv)
{
    SPDLOG_INFO("GPU Raytracer startup");
    Timers timers{};

    if (SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG)
    {
        spdlog::set_level(spdlog::level::debug);
    }

    argparse::ArgumentParser args("vulkan_tracer");
    args.add_argument("scene").help("YML file with scene definition");

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

        VulkanCompute vc{sizeof(InputData), width * height * sizeof(float) * 4, "main.spv", width, height, 1};
        ComputeRunner runner{vc, scene.build()};

        runner.execute(scene.getTargetIterations());

        save(runner.results(), width, height, "output.ppm");
    }
    catch (const std::exception& e)
    {
        SPDLOG_CRITICAL("Execution failed: {}", e.what());
    }

    return 0;
}
