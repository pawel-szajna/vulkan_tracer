#pragma once

#include <chrono>
#include <ostream>
#include <vector>

#include "io_types.hpp"

// TODO: Those all probably have a better place to belong to

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
void changeRandomSeed(InputData& inputs);
void save(const std::vector<float>& data, u32 width, u32 height, std::string_view filename);
unsigned exportColor(float color);
std::tuple<float, float, float> xyzToRgb(float x, float y, float z);
void setupLogger();
