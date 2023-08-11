#include "Arguments.hpp"

#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>

namespace vrt::front
{
bool Arguments::parse(int argc, char** argv)
{
    // clang-format off
    argparse::ArgumentParser args("vulkan_tracer");
    args.add_argument("-i", "--input").help("YML file with scene definition");
    args.add_argument("-p", "--preview").help("enable live preview of the rendered image").default_value(false).implicit_value(true);
    args.add_argument("-s", "--scale").help("scales the output preview window").default_value(1.f).scan<'g', float>();
    args.add_argument("-l", "--list-devices").help("show available devices").default_value(false).implicit_value(true);
    args.add_argument("-d", "--device").help("manually specify a device").default_value(-1).scan<'i', int>();
    args.add_argument("-t", "--time").help("aim for a given execution time per scheduled task [ms]").default_value(150).scan<'i', int>();
    args.add_argument("-v", "--verbose").help("print all logs to console").default_value(false).implicit_value(true);
    args.add_argument("-n", "--no-postprocessing").help("disable postprocessing").default_value(false).implicit_value(true);
    // clang-format on

    try
    {
        args.parse_args(argc, argv);

        input = args.present("--input") ? args.get<std::string>("--input") : std::string{};
        disablePostprocessing = args.get<bool>("--no-postprocessing");
        listDevices = args.get<bool>("--list-devices");
        enablePreview = args.get<bool>("--preview");
        verbose = args.get<bool>("--verbose");
        scale = args.get<float>("--scale");
        device = args.get<i32>("--device");
        timeTarget = args.get<i32>("--time");
    }
    catch (const std::runtime_error& e)
    {
        auto description = std::string_view(e.what());
        if (not description.empty())
        {
            SPDLOG_WARN("Argument parsing problem: {}", description);
        }
        std::cerr << args;
        return false;
    }

    return true;
}
}
