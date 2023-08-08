#include "scene.hpp"

#include <spdlog/spdlog.h>
#include <stdexcept>

#include "constants.hpp"
#include "helpers.hpp"

namespace
{
void assignVector(float* target, vec3 source)
{
    target[0] = source.x;
    target[1] = source.y;
    target[2] = source.z;
}
}

SceneBuilder::SceneBuilder()
{
    changeRandomSeed(inputs);
}

InputData SceneBuilder::build()
{
    auto consumption = inputs.shapesCount + inputs.materialsCount + (vectorId * 4) + scalarId + integerId;

    SPDLOG_INFO("Scene built");
    SPDLOG_INFO("    Memory: {}/{} bytes", 4 * consumption, sizeof(inputs));
    SPDLOG_INFO("    Structures usage:");
    SPDLOG_INFO("        Shapes: {}/{}", inputs.shapesCount, shapesLimit);
    SPDLOG_INFO("        Materials: {}/{}", inputs.materialsCount, materialsLimit);
    SPDLOG_INFO("    Data usage:");
    SPDLOG_INFO("        Vectors: {}/{}", vectorId, vectorsLimit);
    SPDLOG_INFO("        Scalars: {}/{}", scalarId, scalarsLimit);
    SPDLOG_INFO("        Integers: {}/{}", integerId, integersLimit);

    return inputs;
}

u32 SceneBuilder::getResolutionWidth() const
{
    return inputs.renderWidth;
}

u32 SceneBuilder::getResolutionHeight() const
{
    return inputs.renderHeight;
}

void SceneBuilder::setResolution(u32 width, u32 height)
{
    inputs.renderWidth  = width;
    inputs.renderHeight = height;
}

void SceneBuilder::setBackground(vec3 color, float intensity)
{
    assignVector(inputs.background, color);
    inputs.background[3] = intensity;
}

void SceneBuilder::setCamera(vec3 origin, vec3 target, vec3 up, float fov)
{
    assignVector(inputs.cameraOrigin, origin);
    assignVector(inputs.cameraTarget, target);
    assignVector(inputs.cameraUp, up);
    inputs.cameraFov = fov;
}

u32 SceneBuilder::getTargetIterations() const
{
    return targetIterations;
}

u32 SceneBuilder::getMaterialsCount() const
{
    return inputs.materialsCount;
}

void SceneBuilder::setSamplesPerShaderPass(u32 count)
{
    inputs.samplesPerShader = count;
}

void SceneBuilder::setTargetIterations(u32 count)
{
    targetIterations = count;
}

void SceneBuilder::setReflectionsLimit(u32 count)
{
    inputs.reflectionsLimit = count;
}

void SceneBuilder::addMaterialDiffuse(vec3 color)
{
    addVector(color);
    addMaterial(MaterialType::Diffuse);
}

void SceneBuilder::addMaterialMirror(vec3 color)
{
    addVector(color);
    addMaterial(MaterialType::Mirror);
}

void SceneBuilder::addMaterialFog(float intensity)
{
    addScalar(intensity);
    addMaterial(MaterialType::Fog);
}

void SceneBuilder::addMaterialLight(vec3 color, float intensity)
{
    addVector(color);
    addScalar(intensity);
    addMaterial(MaterialType::Light);
}

void SceneBuilder::addMaterialGlass(float refractiveIndex)
{
    addScalar(refractiveIndex);
    addMaterial(MaterialType::Glass);
}

void SceneBuilder::addShapeSphere(vec3 center, float radius, i32 material)
{
    addVector(center);
    addScalar(radius);
    addInteger(material);
    addShape(ShapeType::Sphere);
}

void SceneBuilder::addShapeCloud(vec3 center, float radius, float intensity, i32 material)
{
    addVector(center);
    addScalar(radius);
    addScalar(-1.f / intensity);
    addInteger(material);
    addShape(ShapeType::Cloud);
}

void SceneBuilder::addShapePrism(float top, float bottom, std::vector<vec3> vertices, i32 material)
{
    for (const auto& vertex : vertices)
    {
        addVector(vertex);
    }
    addScalar(top);
    addScalar(bottom);
    addInteger(vertices.size());
    addInteger(material);
    addShape(ShapeType::Prism);
}

void SceneBuilder::addMaterial(MaterialType material)
{
    if (inputs.materialsCount == materialsLimit)
    {
        throw std::length_error("Too many materials");
    }

    if (inputs.shapesCount > 0)
    {
        throw std::logic_error("Materials cannot be added once a shape has been defined");
    }

    inputs.materials[inputs.materialsCount++] = std::to_underlying(material);
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

    inputs.vectors[vectorId * 4]     = vector.x;
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

void SceneBuilder::addInteger(i32 integer)
{
    if (integerId == integersLimit)
    {
        throw std::length_error("Too many integers");
    }

    inputs.integers[integerId++] = integer;
}
