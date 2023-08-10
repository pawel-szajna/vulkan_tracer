#pragma once

#include <string_view>

#include "scene.hpp"

namespace SceneReader
{
SceneBuilder read(std::string_view filename);
}