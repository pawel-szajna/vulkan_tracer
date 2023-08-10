#pragma once

#include <postprocessing/Executor.hpp>

#include <memory>

namespace vrt
{
namespace runner
{
class ComputeRunner;
}

namespace scene
{
class Scene;
}

namespace preview
{
class Window;

class Preview
{
public:

    Preview(const scene::Scene& scene,
            runner::ComputeRunner& runner,
            float scale);
    ~Preview();

    void start();

private:

    const scene::Scene& scene;
    runner::ComputeRunner& runner;
    postprocessing::Executor postprocess;
    std::unique_ptr<Window> window;
};
}
}
