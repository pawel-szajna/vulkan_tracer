#include "timers.hpp"

#include <spdlog/spdlog.h>

Timers::Timers()
    : creation{std::chrono::system_clock::now()}
{
    if (lastInstance != nullptr)
    {
        SPDLOG_WARN("Multiple instances of timers service created, this may result in wrong statistics");
    }

    lastInstance = this;
}

Timers::~Timers()
{
    auto now      = std::chrono::system_clock::now();
    auto duration = (now - creation).count();

    u64 sum{};

    SPDLOG_DEBUG("Execution took {} ms", duration / 1000.0);
    SPDLOG_DEBUG("Time statistics:");
    for (const auto& [type, time] : history)
    {
        SPDLOG_DEBUG("    {}: {} ms total ({:.1f}%)", type, time / 1000.0, 100.0 * time / duration);
        sum += time;
    }
    auto other = duration - sum;
    SPDLOG_DEBUG("    Other: {} ms total ({:.1f}%)", other / 1000.0, 100.0 * other / duration);

    lastInstance = nullptr;
}

TimerInfo Timers::create(std::string name)
{
    if (lastInstance == nullptr)
    {
        SPDLOG_WARN("Timers service does not exist");
    }

    return TimerInfo{std::move(name)};
}

u64 Timers::stop(TimerInfo& timer)
{
    if (lastInstance == nullptr)
    {
        SPDLOG_WARN("Tried to stop a timer, but timers service does not exist");
        return 0;
    }

    return lastInstance->finalize(timer);
}

u64 Timers::finalize(TimerInfo& timer)
{
    auto now      = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - timer.start).count();
    SPDLOG_DEBUG("{} took {} ms", timer.name, duration / 1000.0);
    history[timer.name] += duration;
    return duration;
}

TimerInfo::TimerInfo(std::string name)
    : name{std::move(name)}
    , start{std::chrono::system_clock::now()}
    , finished{false}
{
}

TimerInfo::~TimerInfo()
{
    finish();
}

u64 TimerInfo::finish()
{
    if (not finished)
    {
        finished = true;
        return Timers::stop(*this);
    }

    return 0;
}

Timers* Timers::lastInstance = nullptr;
