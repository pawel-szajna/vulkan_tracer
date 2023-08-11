#include "Scene.hpp"

#include <constants.hpp>
#include <utils/Helpers.hpp>

#include <spdlog/spdlog.h>
#include <stdexcept>

namespace
{
void assignVector(float* target, vec3 source)
{
    target[0] = source.x;
    target[1] = source.y;
    target[2] = source.z;
}
}

namespace vrt::scene
{
Scene::Scene()
{
    changeRandomSeed(inputs);
}

InputData Scene::build()
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

u32 Scene::getResolutionWidth() const
{
    return inputs.renderWidth;
}

u32 Scene::getResolutionHeight() const
{
    return inputs.renderHeight;
}

void Scene::setResolution(u32 width, u32 height)
{
    inputs.renderWidth  = width;
    inputs.renderHeight = height;
}

void Scene::setBackground(vec3 color, float intensity)
{
    assignVector(inputs.background, color);
    inputs.background[3] = intensity;
}

void Scene::setCamera(vec3 origin, vec3 target, vec3 up, float fov)
{
    assignVector(inputs.cameraOrigin, origin);
    assignVector(inputs.cameraTarget, target);
    assignVector(inputs.cameraUp, up);
    inputs.cameraFov = fov;
}

u32 Scene::getTargetIterations() const
{
    return targetIterations;
}

u32 Scene::getMaterialsCount() const
{
    return inputs.materialsCount;
}

void Scene::setSamplesPerShaderPass(u32 count)
{
    inputs.samplesPerShader = count;
}

void Scene::setTargetIterations(u32 count)
{
    targetIterations = count;
}

void Scene::setReflectionsLimit(u32 count)
{
    inputs.reflectionsLimit = count;
}

void Scene::addMaterialDiffuse(vec3 color)
{
    addMaterial(MaterialType::Diffuse);
    addVector(color);
}

void Scene::addMaterialMirror(vec3 color)
{
    addMaterial(MaterialType::Mirror);
    addVector(color);
}

void Scene::addMaterialFog(float intensity)
{
    addMaterial(MaterialType::Fog);
    addScalar(intensity);
}

void Scene::addMaterialLight(vec3 color, float intensity)
{
    addMaterial(MaterialType::Light);
    addVector(color);
    addScalar(intensity);
}

void Scene::addMaterialGlass(float refractiveIndex)
{
    addMaterial(MaterialType::Glass);
    addScalar(refractiveIndex);
}

void Scene::addShapeSphere(vec3 center, float radius, i32 material)
{
    addShape(ShapeType::Sphere);
    addVector(center);
    addScalar(radius);
    addInteger(material);
}

void Scene::addShapeCloud(vec3 center, float radius, float intensity, i32 material)
{
    addShape(ShapeType::Cloud);
    addVector(center);
    addScalar(radius);
    addScalar(-1.f / intensity);
    addInteger(material);
}

void Scene::addShapePrism(float top, float bottom, std::vector<vec3> vertices, i32 material)
{
    addShape(ShapeType::Prism);
    for (const auto& vertex : vertices)
    {
        addVector(vertex);
    }
    addScalar(top);
    addScalar(bottom);
    addInteger(vertices.size());
    addInteger(material);
}

void Scene::addMaterial(MaterialType material)
{
    if (inputs.materialsCount == materialsLimit)
    {
        throw std::length_error("Too many materials");
    }

    inputs.materials[inputs.materialsCount * 4 + 0] = std::to_underlying(material);
    inputs.materials[inputs.materialsCount * 4 + 1] = vectorId;
    inputs.materials[inputs.materialsCount * 4 + 2] = scalarId;
    inputs.materials[inputs.materialsCount * 4 + 3] = integerId;

    inputs.materialsCount++;
}

void Scene::addShape(ShapeType shape)
{
    if (inputs.shapesCount == shapesLimit)
    {
        throw std::length_error("Too many shapes");
    }

    inputs.shapes[inputs.shapesCount * 4 + 0] = std::to_underlying(shape);
    inputs.shapes[inputs.shapesCount * 4 + 1] = vectorId;
    inputs.shapes[inputs.shapesCount * 4 + 2] = scalarId;
    inputs.shapes[inputs.shapesCount * 4 + 3] = integerId;

    inputs.shapesCount++;
}

void Scene::addVector(vec3 vector)
{
    if (vectorId == vectorsLimit)
    {
        throw std::length_error("Too many vectors");
    }

    inputs.vectors[vectorId * 4 + 0] = vector.x;
    inputs.vectors[vectorId * 4 + 1] = vector.y;
    inputs.vectors[vectorId * 4 + 2] = vector.z;

    vectorId++;
}

void Scene::addScalar(float scalar)
{
    if (scalarId == scalarsLimit)
    {
        throw std::length_error("Too many scalars");
    }

    inputs.scalars[scalarId++] = scalar;
}

void Scene::addInteger(i32 integer)
{
    if (integerId == integersLimit)
    {
        throw std::length_error("Too many integers");
    }

    inputs.integers[integerId++] = integer;
}
}
