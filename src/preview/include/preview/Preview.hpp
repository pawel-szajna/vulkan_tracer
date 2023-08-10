#pragma once

#include <memory>

class SceneBuilder;

namespace vrt
{
namespace runner
{
class ComputeRunner;
}

namespace preview
{
class Window;

class Preview
{
public:

    Preview(const SceneBuilder& scene, runner::ComputeRunner& runner, float scale);
    ~Preview();

    void start();

private:

    const SceneBuilder& scene;
    runner::ComputeRunner& runner;
    std::unique_ptr<Window> window;
};
}
}