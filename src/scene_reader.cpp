#include "scene_reader.hpp"

#include <spdlog/spdlog.h>
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

void readSphere(SceneBuilder& scene, const YAML::Node& sphere)
{
    scene.addSphere(readVector(sphere["Center"]), sphere["Radius"].as<float>());
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
} // namespace

SceneBuilder SceneReader::read(std::string_view filename)
{
    SPDLOG_INFO("Loading scene configuration from {}", filename);
    auto data = YAML::LoadFile(std::string(filename));

    SceneBuilder scene;
    readConfiguration(scene, data["Configuration"]);
    readShapes(scene, data["Shapes"]);
    return scene;
}
