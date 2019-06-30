#pragma once

#include "model.hpp"
#include "tinyobjloader/tiny_obj_loader.h"

class Obj : public IModel
{
    tinyobj::ObjReader m_objReader;
    mutable size_t current_shape = 0;
public:
    virtual void ReadModel(const std::filesystem::path& path_to_model) override;
    virtual std::unique_ptr<IShape> GetNextShape() const override;
};

class Shape : public IShape
{
    const std::vector<tinyobj::index_t>& m_indices;
    const std::vector<unsigned char>& m_numFaceVertices;
    const std::vector<float_t>& m_vertices;
    const std::vector<float_t>& m_texCoords;
    mutable size_t current_face = 0;

    vec3f GetVertex(const uint32_t idx) const;
    vec2f GetTextureCoord(const uint32_t idx) const;
public:
    Shape(const tinyobj::shape_t& shape,
        const std::vector<float_t>& vertices,
        const std::vector<float_t>& tex_coords) :
            m_indices(shape.mesh.indices),
            m_numFaceVertices(shape.mesh.num_face_vertices),
            m_vertices(vertices),
            m_texCoords(tex_coords)
    {
    }

    virtual std::optional<TriangulatePolygon> GetNextPolygon() const override;
};
