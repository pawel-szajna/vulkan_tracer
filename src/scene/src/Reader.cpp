#include "Reader.hpp"

#include <algorithm>
#include <iterator>
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
    vrt::scene::Scene operator()();

private:

    void readConfiguration();
    void readBackground();
    void readCamera();

    void readMaterials();
    void readDiffuse(i32 id, const YAML::Node& diffuse);
    void readMirror(i32 id, const YAML::Node& mirror);
    void readFog(i32 id, const YAML::Node& fog);
    void readLight(i32 id, const YAML::Node& light);
    void readGlass(i32 id, const YAML::Node& glass);

    void readShapes();
    void readShape(const YAML::Node& shape);
    void readSphere(const YAML::Node& sphere);
    void readCloud(const YAML::Node& cloud);
    void readPrism(const YAML::Node& prism);

    i32 readMaterial(const YAML::Node& source);
    void saveMaterial(i32 id, const YAML::Node& node);

    YAML::Node& root;
    vrt::scene::Scene scene;
    std::map<std::string, i32> materialNames;
};

SceneReaderImpl::SceneReaderImpl(YAML::Node& root)
    : root{root}
    , scene{}
{}

vrt::scene::Scene SceneReaderImpl::operator()()
{
    try
    {
        readConfiguration();
        readCamera();
        readBackground();
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
    scene.setTargetIterations(config["TotalSamples"].as<u32>());
    scene.setReflectionsLimit(config["ReflectionsLimit"].as<u32>());
}

void SceneReaderImpl::readBackground()
{
    auto background = root["Background"];

    scene.setBackground(readVector(background["Color"]),
                        background["Intensity"].as<float>());
}

void SceneReaderImpl::readCamera()
{
    auto camera = root["Camera"];

    scene.setCamera(readVector(camera["Origin"]),
                    readVector(camera["Target"]),
                    readVector(camera["Up"]),
                    camera["FieldOfVision"].as<float>());
}

i32 SceneReaderImpl::readMaterial(const YAML::Node& source)
{
    i32 materialId;

    try
    {
        materialId = source.as<i32>();
    }
    catch (const YAML::Exception&)
    {
        auto name = source.as<std::string>();
        if (not materialNames.contains(name))
        {
            throw std::invalid_argument("Invalid material name specified");
        }
        materialId = materialNames.at(source.as<std::string>());
    }

    if (scene.getMaterialsCount() < materialId + 1)
    {
        throw std::out_of_range(fmt::format("Requested material index {} out of range", materialId));
    }
    return materialId;
}

void SceneReaderImpl::saveMaterial(i32 id, const YAML::Node& node)
{
    materialNames.emplace(node["Name"].as<std::string>(), id);
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

    if (shape["Prism"])
    {
        readPrism(shape["Prism"]);
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

void SceneReaderImpl::readPrism(const YAML::Node& prism)
{
    std::vector<vec3> vertices;
    std::transform(prism["Vertices"].begin(),
                   prism["Vertices"].end(),
                   std::back_inserter(vertices),
                   [](const YAML::Node& vertex) { return readVector(vertex); });

    scene.addShapePrism(prism["Top"].as<float>(),
                        prism["Bottom"].as<float>(),
                        std::move(vertices),
                        readMaterial(prism["Material"]));
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

        if (material["Light"])
        {
            readLight(id++, material["Light"]);
            continue;
        }

        if (material["Glass"])
        {
            readGlass(id++, material["Glass"]);
            continue;
        }
    }
}

void SceneReaderImpl::readDiffuse(i32 id, const YAML::Node& diffuse)
{
    saveMaterial(id, diffuse);
    scene.addMaterialDiffuse(readVector(diffuse["Color"]));
}

void SceneReaderImpl::readMirror(i32 id, const YAML::Node& mirror)
{
    saveMaterial(id, mirror);
    scene.addMaterialMirror(readVector(mirror["Color"]));
}

void SceneReaderImpl::readFog(i32 id, const YAML::Node& fog)
{
    saveMaterial(id, fog);
    scene.addMaterialFog(fog["Intensity"].as<float>());
}

void SceneReaderImpl::readLight(i32 id, const YAML::Node& light)
{
    saveMaterial(id, light);
    scene.addMaterialLight(readVector(light["Color"]),
                           light["Intensity"].as<float>());
}

void SceneReaderImpl::readGlass(i32 id, const YAML::Node& glass)
{
    saveMaterial(id, glass);
    scene.addMaterialGlass(glass["RefractiveIndex"].as<float>());
}
} // namespace

namespace vrt::scene
{
Scene read(std::string_view filename)
{
    SPDLOG_INFO("Loading scene configuration from {}", filename);
    auto data = YAML::LoadFile(std::string(filename));
    return ::SceneReaderImpl{data}();
}
}
