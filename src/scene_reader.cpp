#include "scene_reader.hpp"

#include <map>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <yaml-cpp/yaml.h>

namespace
{
vec3 readVector(const YAML::Node& source)
{
    return {source[0].as<float>(), source[1].as<float>(), source[2].as<float>()};
}

class SceneReaderImpl
{
public:

    explicit SceneReaderImpl(YAML::Node& root);
    SceneBuilder operator()();

private:

    void readConfiguration();

    void readMaterials();
    void readDiffuse(i32 id, const YAML::Node& diffuse);
    void readMirror(i32 id, const YAML::Node& mirror);
    void readFog(i32 id, const YAML::Node& fog);

    void readShapes();
    void readShape(const YAML::Node& shape);
    void readSphere(const YAML::Node& sphere);
    void readCloud(const YAML::Node& cloud);

    i32 readMaterial(const YAML::Node& source);

    YAML::Node& root;
    SceneBuilder scene;
    std::map<std::string, i32> materialNames;
};

SceneReaderImpl::SceneReaderImpl(YAML::Node& root)
    : root{root}
    , scene{}
{}

SceneBuilder SceneReaderImpl::operator()()
{
    try
    {
        readConfiguration();
        readMaterials();
        readShapes();
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

void SceneReaderImpl::readConfiguration()
{
    auto config = root["Configuration"];

    scene.setResolution(config["Resolution"]["Width"].as<u32>(), config["Resolution"]["Height"].as<u32>());
    scene.setSamplesPerShaderPass(config["SamplesPerShader"].as<u32>());
    scene.setTargetIterations(config["TotalSamples"].as<u32>());
    scene.setReflectionsLimit(config["ReflectionsLimit"].as<u32>());
}

i32 SceneReaderImpl::readMaterial(const YAML::Node& source)
{
    i32 materialId;

    try
    {
        materialId = source.as<i32>();
    }
    catch(YAML::Exception&)
    {
        materialId = materialNames.at(source.as<std::string>());
    }

    if (scene.getMaterialsCount() < materialId + 1)
    {
        throw std::out_of_range(fmt::format("Requested material index {} out of range", materialId));
    }
    return materialId;
}

void SceneReaderImpl::readShapes()
{
    for (const auto& shape : root["Shapes"])
    {
        readShape(shape);
    }
}

void SceneReaderImpl::readShape(const YAML::Node& shape)
{
    if (shape["Sphere"])
    {
        readSphere(shape["Sphere"]);
        return;
    }

    if (shape["Cloud"])
    {
        readCloud(shape["Cloud"]);
        return;
    }
}

void SceneReaderImpl::readSphere(const YAML::Node& sphere)
{
    scene.addShapeSphere(readVector(sphere["Center"]),
                         sphere["Radius"].as<float>(),
                         readMaterial(sphere["Material"]));
}

void SceneReaderImpl::readCloud(const YAML::Node& cloud)
{
    scene.addShapeCloud(readVector(cloud["Center"]),
                        cloud["Radius"].as<float>(),
                        cloud["Density"].as<float>(),
                        readMaterial(cloud["Material"]));
}

void SceneReaderImpl::readMaterials()
{
    int id = 0;
    auto materials = root["Materials"];

    for (const auto& material : materials)
    {
        if (material["Diffuse"])
        {
            readDiffuse(id++, material["Diffuse"]);
            continue;
        }

        if (material["Mirror"])
        {
            readMirror(id++, material["Mirror"]);
            continue;
        }

        if (material["Fog"])
        {
            readFog(id++, material["Fog"]);
            continue;
        }
    }
}

void SceneReaderImpl::readDiffuse(i32 id, const YAML::Node& diffuse)
{
    materialNames.emplace(diffuse["Name"].as<std::string>(), id);
    scene.addMaterialDiffuse(readVector(diffuse["Color"]));
}

void SceneReaderImpl::readMirror(i32 id, const YAML::Node& mirror)
{
    materialNames.emplace(mirror["Name"].as<std::string>(), id);
    scene.addMaterialMirror(readVector(mirror["Color"]));
}

void SceneReaderImpl::readFog(i32 id, const YAML::Node& fog)
{
    materialNames.emplace(fog["Name"].as<std::string>(), id);
    scene.addMaterialFog(fog["Intensity"].as<float>());
}
} // namespace

SceneBuilder SceneReader::read(std::string_view filename)
{
    SPDLOG_INFO("Loading scene configuration from {}", filename);
    auto data = YAML::LoadFile(std::string(filename));
    return SceneReaderImpl{data}();
}
