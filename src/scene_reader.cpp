#include "scene_reader.hpp"

#include <spdlog/spdlog.h>
#include <stdexcept>
#include <yaml-cpp/yaml.h>

namespace
{
void readConfiguration(SceneBuilder& scene, const YAML::Node& config)
{
    scene.setResolution(config["Resolution"]["Width"].as<u32>(), config["Resolution"]["Height"].as<u32>());
    scene.setSamplesPerShaderPass(config["SamplesPerShader"].as<u32>());
    scene.setTargetIterations(config["TotalSamples"].as<u32>());
    scene.setReflectionsLimit(config["ReflectionsLimit"].as<u32>());
}

vec3 readVector(const YAML::Node& source)
{
    return {source[0].as<float>(), source[1].as<float>(), source[2].as<float>()};
}

i32 readMaterial(SceneBuilder& scene, const YAML::Node& source)
{
    auto materialId = source.as<i32>();
    if (scene.getMaterialsCount() < materialId + 1)
    {
        throw std::out_of_range(fmt::format("Requested material index {} out of range", materialId));
    }
    return materialId;
}

void readSphere(SceneBuilder& scene, const YAML::Node& sphere)
{
    scene.addShapeSphere(readVector(sphere["Center"]),
                         sphere["Radius"].as<float>(),
                         readMaterial(scene, sphere["Material"]));
}

void readShapes(SceneBuilder& scene, const YAML::Node& shapes)
{
    for (const auto& shape : shapes)
    {
        if (shape["Sphere"])
        {
            readSphere(scene, shape["Sphere"]);
            continue;
        }
    }
}

void readDiffuse(SceneBuilder& scene, const YAML::Node& diffuse)
{
    scene.addMaterialDiffuse(readVector(diffuse["Color"]));
}

void readMaterials(SceneBuilder& scene, const YAML::Node& materials)
{
    for (const auto& material : materials)
    {
        if (material["Diffuse"])
        {
            readDiffuse(scene, material["Diffuse"]);
            continue;
        }
    }
}
} // namespace

SceneBuilder SceneReader::read(std::string_view filename)
{
    SPDLOG_INFO("Loading scene configuration from {}", filename);
    auto data = YAML::LoadFile(std::string(filename));

    SceneBuilder scene;
    try
    {
        readConfiguration(scene, data["Configuration"]);
        readMaterials(scene, data["Materials"]);
        readShapes(scene, data["Shapes"]);
    }
    catch (const YAML::Exception& e)
    {
        SPDLOG_ERROR("Problem while reading scene: {}", e.what());
        throw std::runtime_error("Could not read scene data");
    }
    catch (const std::exception& e)
    {
        SPDLOG_ERROR("Problem while constructing scene: {}", e.what());
        throw std::runtime_error("Could not build scene");
    }
    return scene;
}
