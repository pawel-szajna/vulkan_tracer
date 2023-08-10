#pragma once

#include <memory>

class ComputeRunner;
class SceneBuilder;

namespace vrt::preview
{
class Window;

class Preview
{
public:

    Preview(const SceneBuilder& scene, ComputeRunner& runner, float scale);
    ~Preview();

    void start();

private:

    const SceneBuilder& scene;
    ComputeRunner& runner;
    std::unique_ptr<Window> window;
};
}
