#include "helpers.hpp"
#include "io_types.hpp"
#include "live_view.hpp"
#include "runner.hpp"
#include "scene_reader.hpp"
#include "timers.hpp"
#include "vulkan_compute.hpp"

#include <memory>
#include <thread>

#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#if defined(__unix__) or defined(__APPLE__)
#include <unistd.h>
#else
#define isatty(...) false
#endif

int main(int argc, char** argv)
{
    if (isatty(fileno(stdout)))
    {
        auto fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("vulkan_tracer.log");
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        fileSink->set_level(spdlog::level::debug);
        consoleSink->set_level(spdlog::level::warn);
        auto logger = std::make_shared<spdlog::logger>("tracer", spdlog::sinks_init_list{fileSink, consoleSink});
        logger->set_level(spdlog::level::debug);
        spdlog::set_default_logger(logger);
    }

    SPDLOG_INFO("GPU Raytracer startup");
    Timers timers{};

    argparse::ArgumentParser args("vulkan_tracer");
    args.add_argument("files").help("YML file with scene definition").remaining();
    args.add_argument("--preview").help("enable live preview of the rendered image").default_value(false).implicit_value(true);
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
    auto preview = args.get<bool>("--preview");

    if (preview and files.size() > 1)
    {
        SPDLOG_WARN("Live preview can be enabled only if a single image is rendered");
        preview = false;
    }

    for (const auto& file : files)
    {
        auto scene = SceneReader::read(file);

        auto width  = scene.getResolutionWidth();
        auto height = scene.getResolutionHeight();

        if (width % 64 != 0 or height % 64 != 0)
        {
            SPDLOG_WARN("Image resolution {}x{} is not divisible by 64, reducing to {}x{}",
                        width, height, width / 64 * 64, height / 64 * 64);
            width = width / 64 * 64;
            height = height / 64 * 64;
            scene.setResolution(width, height);
        }

        VulkanCompute vc{sizeof(InputData), width * height * sizeof(float) * 4, "main.spv", 64, 64, 1, deviceId};
        ComputeRunner runner{vc, scene.build(), file};

        std::thread runnerThread([&]() { runner.execute(scene.getTargetIterations()); });

        if (preview)
        {
            LiveView{scene, runner}.start();
        }

        if (runnerThread.joinable())
        {
            runnerThread.join();
        }

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
