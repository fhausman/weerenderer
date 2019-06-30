#pragma once

#include "hola/hola.hpp"
#include <filesystem>
#include <optional>

using namespace hola;

using Vertices = std::array<vec3f, 3>;
using TextureCoords = std::array<vec2f, 3>;

struct TriangulatePolygon
{
    Vertices vertices;
    TextureCoords textureCoordinates;
};

struct IShape
{
    virtual std::optional<TriangulatePolygon> GetNextPolygon() const = 0;
    virtual ~IShape() = default;
};

struct IModel
{
    virtual void ReadModel(const std::filesystem::path& path_to_model) = 0;
    virtual std::unique_ptr<IShape> GetNextShape() const = 0;
    virtual ~IModel() = default;
};
