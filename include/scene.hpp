#pragma once

#include "io_types.hpp"

enum class ShapeType : uint32_t
{
    Invalid,
    Sphere,
};

class SceneBuilder
{
public:

    SceneBuilder();

    InputData build();

    void setResolution(u32 width, u32 height);
    void setSamplesPerShaderPass(u32 count);

    void addSphere(vec3 center, float radius);

private:

    void addShape(ShapeType shape);
    void addVector(vec3 vector);
    void addScalar(float scalar);

    InputData inputs{};
    usize vectorId{};
    usize scalarId{};
};
