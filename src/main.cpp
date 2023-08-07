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

    #if defined(DebugBuild)
    spdlog::set_level(spdlog::level::debug);
    #endif

    argparse::ArgumentParser args("vulkan_tracer");
    args.add_argument("files").help("YML file with scene definition").remaining();
    args.add_argument("--list-devices").help("show available devices").default_value(false).implicit_value(true);
    args.add_argument("--device").help("manually specify a device").default_value(-1).scan<'i', int>();

    try
    {
        args.parse_args(argc, argv);
    }
    catch (const std::runtime_error& e)
    {
        auto description = std::string_view(e.what());
        if (not description.empty())
        {
            SPDLOG_WARN("Argument parsing problem: {}", description);
        }
        std::cerr << args;
        return 1;
    }

    if (args.get<bool>("--list-devices"))
    {
        VulkanCompute::listDevices();
        return 0;
    }

    #if not defined(DebugBuild)
    try
    {
    #endif

    std::vector<std::string> files;
    try
    {
        files = args.get<std::vector<std::string>>("files");
        SPDLOG_DEBUG("{} files to process", files.size());
    }
    catch (const std::logic_error&)
    {
        SPDLOG_WARN("Nothing to do. Call with --help to check usage");
        return 0;
    }

    auto deviceId = args.get<int>("--device");

    for (const auto& file : files)
    {
        auto scene = SceneReader::read(file);

        auto width  = scene.getResolutionWidth();
        auto height = scene.getResolutionHeight();

        VulkanCompute vc{sizeof(InputData), width * height * sizeof(float) * 4, "main.spv", 64, 64, 1, deviceId};
        ComputeRunner runner{vc, scene.build()};

        runner.execute(scene.getTargetIterations());

        save(runner.results(), width, height, fmt::format("{}_output.ppm", file));
    }

    #if not defined(DebugBuild)
    }
    catch (const std::exception& e)
    {
        SPDLOG_CRITICAL("Execution failed: {}", e.what());
        throw;
    }
    #endif

    return 0;
}
