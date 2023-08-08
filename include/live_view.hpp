#pragma once

#include <memory>

class ComputeRunner;
class LiveViewWindow;
class SceneBuilder;

class LiveView
{
public:

    LiveView(const SceneBuilder& scene, ComputeRunner& runner, float scale);
    ~LiveView();

    void start();

private:

    const SceneBuilder& scene;
    ComputeRunner& runner;
    std::unique_ptr<LiveViewWindow> window;
};
