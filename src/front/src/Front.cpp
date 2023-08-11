#include "Arguments.hpp"

#include <io_types.hpp>
#include <postprocessing/Executor.hpp>
#include <preview/Preview.hpp>
#include <runner/ComputeRunner.hpp>
#include <runner/VulkanCompute.hpp>
#include <scene/Reader.hpp>
#include <utils/Helpers.hpp>
#include <utils/Timers.hpp>

#include <thread>

#include <spdlog/spdlog.h>

namespace vrt::front
{
int start(int argc, char** argv)
{
    Timers timers{};
    Arguments args{};

    if (not args.parse(argc, argv))
    {
        return 1;
    }

    if (not args.verbose)
    {
        setupLogger();
    }
    else
    {
        spdlog::set_level(spdlog::level::debug);
    }

    if (args.listDevices)
    {
        runner::VulkanCompute::listDevices();
        return 0;
    }

    #if not defined(DebugBuild)
    try
    {
    #endif

    if (args.input.empty())
    {
        SPDLOG_WARN("Nothing to do. Call with --help to check usage");
        return 0;
    }

    auto scene = vrt::scene::read(args.input);

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

    runner::VulkanCompute vc{sizeof(InputData), width * height * sizeof(float) * 4, "main.spv", 64, 64, 1, args.device};
    runner::ComputeRunner runner{vc, scene.build(), args.input, args.timeTarget, not args.verbose};
    runner.onDone([&](runner::ComputeRunner& runner) { save(runner.results(), width, height, fmt::format("{}_output.ppm", args.input)); });
    std::thread runnerThread([&]() { runner.execute(scene.getTargetIterations()); });
    postprocessing::Executor postprocess{};

    if (args.enablePreview)
    {
        preview::Preview{scene, runner, args.scale, not args.disablePostprocessing}.start();
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
