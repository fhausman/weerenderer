#pragma once

#include "hola/hola.hpp"
#include <filesystem>
#include <optional>

using Vertices = std::array<vec3f, 3>;
using TextureCoords = std::array<vec2f, 3>;

struct Polygon
{
    Vertices vertices;
    TextureCoords textureCoordinates;
};

class IShape
{
    virtual std::optional<Polygon> GetNextPolygon() = 0;
    virtual ~IShape() = default;
};

class IModel
{
public:
    virtual void ReadModel(const std::filesystem::path& path_to_model) = 0;
    virtual std::unique_ptr<IShape> GetNextShape() = 0;
    virtual ~IModel() = default;
};
