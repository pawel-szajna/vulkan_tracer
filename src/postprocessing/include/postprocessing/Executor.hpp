#pragma once

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

namespace vrt::postprocessing
{
class Task;

class Executor
{
public:

    Executor();
    ~Executor();

    void execute(std::unique_ptr<Task> task);

private:

    std::thread createWorker();

    std::atomic<bool> running{true};
    std::vector<std::thread> workers{};
    std::unique_ptr<Task> currentTask{nullptr};

};
}
