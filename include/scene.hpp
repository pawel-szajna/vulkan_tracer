#pragma once

#include "helpers.hpp"
#include "io_types.hpp"

#include <iostream>

enum class ShapeType : uint32_t
{
    Invalid,
    Sphere,
};

class RaytracerInputBuilder
{
public:

    RaytracerInputBuilder()
    {
        seedFromClock(inputs);
    }

    InputData build()
    {
        std::cout << "Scene built" << std::endl
                  << "    Shapes: " << inputs.shapesCount << "/" << shapesLimit << std::endl
                  << "    Vectors: " << vectorId << "/" << vectorsLimit << std::endl
                  << "    Scalars: " << scalarId << "/" << scalarsLimit << std::endl;

        InputData result = inputs;
        inputs = InputData{};
        return result;
    }

    void addSphere(vec3 center, float radius)
    {
        addVector(center);
        addScalar(radius);
        addShape(ShapeType::Sphere);
    }

private:

    void addShape(ShapeType shape)
    {
        if (inputs.shapesCount == shapesLimit)
        {
            throw std::length_error("Too many shapes");
        }

        inputs.shapes[inputs.shapesCount++] = std::to_underlying(shape);
    }

    void addVector(vec3 vector)
    {
        if (vectorId == vectorsLimit)
        {
            throw std::length_error("Too many vectors");
        }

        inputs.vectors[vectorId * 4] = vector.x;
        inputs.vectors[vectorId * 4 + 1] = vector.y;
        inputs.vectors[vectorId * 4 + 2] = vector.z;

        vectorId++;
    }

    void addScalar(float scalar)
    {
        if (scalarId == scalarsLimit)
        {
            throw std::length_error("Too many scalars");
        }

        inputs.scalars[scalarId++] = scalar;
    }

    InputData inputs{};
    usize vectorId{};
    usize scalarId{};
};
