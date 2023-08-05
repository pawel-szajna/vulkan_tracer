#pragma once

#include <chrono>
#include <ostream>

#include "io_types.hpp"

template<typename T>
struct Printable
{
    const T* array{};
    usize size{};
    usize grouping{1};
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const Printable<T>& t)
{
    for (usize i = 0; i < t.size; ++i)
    {
        if (t.grouping > 1)
        {
            os << "[ ";
            for (usize j = 0; j < t.grouping; ++j)
            {
                os << t.array[i * t.grouping + j] << ' ';
            }
            os << "] ";
        }
        else
        {
            os << t.array[i * t.grouping] << ' ';
        }
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const ioVec& v);
std::ostream& operator<<(std::ostream& os, const InputData& inputs);
void seedFromClock(InputData& inputs);
void save(const OutputData& data, std::string_view filename);

#if SPDLOG_ACTIVE_LEVEL == SPDLOG_LEVEL_DEBUG

#define TIMER_START \
    auto start = std::chrono::system_clock::now();

#define TIMER_END(what) \
    auto end = std::chrono::system_clock::now(); \
    SPDLOG_DEBUG("{} took {} ms",                 \
                 what,                            \
                 std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0);

#else
#define TIMER_START (void)0;
#define TIMER_END (void)0;
#endif