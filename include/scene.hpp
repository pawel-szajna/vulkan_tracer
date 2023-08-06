#pragma once

#include "io_types.hpp"

enum class ShapeType : uint32_t
{
    Invalid,
    Sphere,
};

enum class MaterialType : uint32_t
{
    Invalid,
    Diffuse,
};

class SceneBuilder
{
public:

    SceneBuilder();

    InputData build();

    void setResolution(u32 width, u32 height);
    void setSamplesPerShaderPass(u32 count);
    void setTargetIterations(u32 count);
    void setReflectionsLimit(u32 count);

    void addMaterialDiffuse(vec3 color);
    void addShapeSphere(vec3 center, float radius, i32 material);

    [[nodiscard]] u32 getResolutionWidth() const;
    [[nodiscard]] u32 getResolutionHeight() const;
    [[nodiscard]] u32 getTargetIterations() const;
    [[nodiscard]] u32 getMaterialsCount() const;

private:

    void addMaterial(MaterialType material);
    void addShape(ShapeType shape);
    void addVector(vec3 vector);
    void addScalar(float scalar);
    void addInteger(i32 integer);

    InputData inputs{};

    usize vectorId{};
    usize scalarId{};
    usize integerId{};

    u32 targetIterations{};
};
