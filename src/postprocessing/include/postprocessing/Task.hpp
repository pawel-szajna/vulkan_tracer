#pragma once

#include <functional>

namespace vrt::postprocessing
{
class Task
{
public:

    using Work = std::function<void()>;

    virtual ~Task() = default;

    virtual void start() = 0;
    virtual bool done() = 0;
    virtual Work get() = 0;
};
}
