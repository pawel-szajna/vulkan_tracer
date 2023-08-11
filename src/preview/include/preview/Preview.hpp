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
            float scale,
            bool enablePostprocessing);
    ~Preview();

    void start();

private:

    const scene::Scene& scene;
    runner::ComputeRunner& runner;
    postprocessing::Executor postprocess;
    bool postprocessingEnabled;
    std::unique_ptr<Window> window;
};
}
}
