#include <vector>
#include <optional>
#include <string_view>

struct VertexIndices
{
    using data_type = float;

    data_type x;
    data_type y;
    data_type z;
    data_type w = 0.;

    VertexIndices() = delete;
};

struct VertexTextureCoordinates
{
    using data_type = float;

    data_type u;
    data_type v = 0.;
    data_type w = 0.;

    VertexTextureCoordinates() = delete;
};

struct VertexNormals
{
    using data_type = float;

    data_type x;
    data_type y;
    data_type z;

    VertexNormals() = delete;
};

struct FaceElementCoords
{
    using data_type = size_t;
    using data_type_opt = std::optional<data_type>;

    data_type vertex_indices;
    data_type_opt vertex_texture_coordinates;
    data_type_opt vertex_normals;

    FaceElementCoords(
        const data_type& vi, const data_type_opt& vt, const data_type_opt& vn)
        : vertex_indices(vi), vertex_texture_coordinates(vt), vertex_normals(vn)
    {
    }
};

struct FaceElements
{
    std::vector<FaceElementCoords> face_element_coords;

    void add(const FaceElementCoords::data_type vertex_indices,
        const FaceElementCoords::data_type_opt vertex_texture_coordinates =
            std::nullopt,
        const FaceElementCoords::data_type_opt vertex_normals = std::nullopt)
    {
        face_element_coords.emplace_back(
            vertex_indices, vertex_texture_coordinates, vertex_normals);
    }
};

class Obj
{
    std::vector<VertexIndices> vertex_indices;
    std::vector<VertexTextureCoordinates> vertex_texture_coordinates;
    std::vector<VertexNormals> vertex_normals;
    std::vector<FaceElements> face_elements;

   public:
    static Obj CreateObjModel(const std::string_view file_dir);
};
