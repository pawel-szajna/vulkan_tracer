#pragma once

#include "Scene.hpp"

#include <string_view>

namespace vrt::scene
{
Scene read(std::string_view filename);
}
