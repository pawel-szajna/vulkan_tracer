#pragma once

#include <string>

#include <types/Basic.hpp>

namespace vrt::front
{
struct Arguments
{
    std::string input;

    bool disablePostprocessing;
    bool listDevices;
    bool enablePreview;
    bool verbose;

    float scale;

    i32 device;
    i32 timeTarget;

    bool parse(int argc, char** argv);
};
}
