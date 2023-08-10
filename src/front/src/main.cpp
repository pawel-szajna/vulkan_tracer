#include <io_types.hpp>
#include <preview/Preview.hpp>
#include <runner/ComputeRunner.hpp>
#include <runner/VulkanCompute.hpp>
#include <scene/Reader.hpp>
#include <utils/Helpers.hpp>
#include <utils/Timers.hpp>

#include <memory>
#include <thread>

#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>

namespace vrt
{
int main(int argc, char** argv)
{
    Timers timers{};
    setupLogger();

    SPDLOG_INFO("GPU Raytracer startup");

    argparse::ArgumentParser args("vulkan_tracer");
    args.add_argument("-i", "--input").help("YML file with scene definition");
    args.add_argument("-p", "--preview").help("enable live preview of the rendered image").default_value(false).implicit_value(true);
    args.add_argument("-s", "--scale").help("scales the output preview window").default_value(1.f).scan<'g', float>();
    args.add_argument("-l", "--list-devices").help("show available devices").default_value(false).implicit_value(true);
    args.add_argument("-d", "--device").help("manually specify a device").default_value(-1).scan<'i', int>();
    args.add_argument("-t", "--time").help("aim for a given execution time per scheduled task [ms]").default_value(150).scan<'i', int>();

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
        runner::VulkanCompute::listDevices();
        return 0;
    }

    #if not defined(DebugBuild)
    try
    {
    #endif

    if (not args.present("--input"))
    {
        SPDLOG_WARN("Nothing to do. Call with --help to check usage");
        return 0;
    }

    auto schedulerTimeTarget = args.get<int>("--time");
    auto deviceId = args.get<int>("--device");
    auto preview = args.get<bool>("--preview");
    auto file = args.get<std::string>("--input");
    auto scene = vrt::scene::read(file);

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

    runner::VulkanCompute vc{sizeof(InputData), width * height * sizeof(float) * 4, "main.spv", 64, 64, 1, deviceId};
    runner::ComputeRunner runner{vc, scene.build(), file, schedulerTimeTarget};
    runner.onDone([&](runner::ComputeRunner& runner) { save(runner.results(), width, height, fmt::format("{}_output.ppm", file)); });
    std::thread runnerThread([&]() { runner.execute(scene.getTargetIterations()); });

    if (preview)
    {
        auto scale = args.get<float>("--scale");
        preview::Preview{scene, runner, scale}.start();
    }

    if (runnerThread.joinable())
    {
        runnerThread.join();
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
}

int main(int argc, char** argv)
{
    return vrt::main(argc, argv);
}
