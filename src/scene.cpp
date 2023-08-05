#include "scene.hpp"

#include <stdexcept>
#include <spdlog/spdlog.h>

#include "constants.hpp"
#include "helpers.hpp"

SceneBuilder::SceneBuilder()
{
    seedFromClock(inputs);
}

InputData SceneBuilder::build()
{
    SPDLOG_INFO("Scene built");
    SPDLOG_INFO("    Shapes: {}/{}", inputs.shapesCount, shapesLimit);
    SPDLOG_INFO("    Vectors: {}/{}", vectorId, vectorsLimit);
    SPDLOG_INFO("    Scalars: {}/{}", scalarId, scalarsLimit);

    InputData result = inputs;
    inputs = InputData{};
    return result;
}

void SceneBuilder::setResolution(u32 width, u32 height)
{
    inputs.renderWidth = width;
    inputs.renderHeight = height;
}

void SceneBuilder::setSamplesPerShaderPass(u32 count)
{
    inputs.samplesPerShader = count;
}

void SceneBuilder::addSphere(vec3 center, float radius)
{
    addVector(center);
    addScalar(radius);
    addShape(ShapeType::Sphere);
}

void SceneBuilder::addShape(ShapeType shape)
{
    if (inputs.shapesCount == shapesLimit)
    {
        throw std::length_error("Too many shapes");
    }

    inputs.shapes[inputs.shapesCount++] = std::to_underlying(shape);
}

void SceneBuilder::addVector(vec3 vector)
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

void SceneBuilder::addScalar(float scalar)
{
    if (scalarId == scalarsLimit)
    {
        throw std::length_error("Too many scalars");
    }

    inputs.scalars[scalarId++] = scalar;
}
