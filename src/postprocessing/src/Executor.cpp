#include "Executor.hpp"

#include "Task.hpp"

#include <algorithm>
#include <chrono>
#include <mutex>
#include <spdlog/spdlog.h>

namespace
{
std::mutex mutex{};
}

namespace vrt::postprocessing
{
Executor::Executor()
{
    auto threadCount = std::max(1u, std::thread::hardware_concurrency() - 1u);
    SPDLOG_INFO("Starting {} workers for postprocessing tasks", threadCount);
    for (int i = 0; i < threadCount; ++i)
    {
        workers.push_back(createWorker());
    }
}

Executor::~Executor()
{
    SPDLOG_INFO("Postprocessing executor shutdown, waiting for workers to finish already started tasks");
    running = false;

    for (auto& worker : workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
}

std::thread Executor::createWorker()
{
    return std::thread([&]() {
        while (running)
        {
            std::optional<Task::Work> job{std::nullopt};
            {
                std::lock_guard lock{mutex};
                if (currentTask != nullptr and not currentTask->done())
                {
                    job = currentTask->get();
                }
            }

            if (job.has_value())
            {
                (*job)();
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    });
}

void Executor::execute(std::unique_ptr<Task> task)
{
    auto start = std::chrono::system_clock::now();
    SPDLOG_DEBUG("Starting a task, blocking");

    while (true)
    {
        {
            std::lock_guard lock{mutex};
            if (currentTask == nullptr)
            {
                currentTask = std::move(task);
                currentTask->start();
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::lock_guard lock{mutex};
        if (currentTask->done())
        {
            SPDLOG_DEBUG("Task finished in {} ms",
                         std::chrono::duration_cast<std::chrono::milliseconds>(
                             std::chrono::system_clock::now() - start).count());
            currentTask.reset();
            return;
        }
    }
}
}
