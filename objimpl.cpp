#include "objimpl.hpp"

void Obj::ReadModel(const std::filesystem::path& path_to_model)
{
    if (path_to_model.extension() != ".obj")
        throw std::runtime_error("Invalid file provided, should be .obj");

    m_objReader.ParseFromFile(path_to_model.string());
    if (!m_objReader.Valid())
        throw std::runtime_error("Failed to read .obj file, reason:\n" + m_objReader.Error());
}

std::unique_ptr<IShape> Obj::GetNextShape() const
{
    const auto shapes_it = m_objReader.GetShapes().begin() + current_shape;
    if (shapes_it != m_objReader.GetShapes().end())
    {
        ++current_shape;
        return std::make_unique<Shape>(
            *shapes_it, m_objReader.GetAttrib().vertices, m_objReader.GetAttrib().texcoords);
    }
    else
    {
        return nullptr;
    }
}

vec3f Shape::GetVertex(const uint32_t idx) const
{
    const auto start_idx = idx * 3;
    return vec3f{ m_vertices[start_idx],
        m_vertices[start_idx + 1],
        m_vertices[start_idx + 2] };
}

vec2f Shape::GetTextureCoord(const uint32_t idx) const
{
    const auto start_idx = idx * 2;
    return vec2f{ m_texCoords[start_idx], m_texCoords[start_idx + 1] };
}

std::optional<TriangulatePolygon> Shape::GetNextPolygon() const
{
    auto polygon = TriangulatePolygon{};
    const auto faces_it = m_numFaceVertices.begin() + current_face;
    if (faces_it != m_numFaceVertices.end())
    {
        const auto face_size = *faces_it;
        if (face_size != 3)
            throw std::runtime_error("Obj file not triangulated, aborting...");

        const auto face = m_indices.begin() + current_face*face_size;
        for (int i = 0; i < face_size; ++i)
        {
            polygon.vertices[i] = GetVertex(face[i].vertex_index);
            polygon.textureCoordinates[i] = GetTextureCoord(face[i].texcoord_index);
        }

        ++current_face;
        return polygon;
    }
    else
    {
        return std::nullopt;
    }
}
