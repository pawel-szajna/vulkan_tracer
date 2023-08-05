#pragma once

#include <chrono>
#include <map>
#include <string>

#include "basic_types.hpp"

using Timestamp = std::chrono::time_point<std::chrono::system_clock>;

struct TimerInfo
{
    std::string name;
    Timestamp start;
    bool finished;

    explicit TimerInfo(std::string name);
    ~TimerInfo();

    u64 finish();
};

class Timers
{
public:

    Timers();
    ~Timers();

    static TimerInfo create(std::string name);
    static u64 stop(TimerInfo& timer);

private:

    u64 finalize(TimerInfo& timer);

    static Timers* lastInstance;
    Timestamp creation;
    std::map<std::string, u64> history;
};
