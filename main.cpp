#include "renderer.hpp"
#include "img/tgaimage.h"
#include "tinyobjloader/tiny_obj_loader.h"
#include "hola/hola.hpp"

#include <iostream>

using namespace hola;

struct ImageReader
{
    const char* img_name;

    void operator()(TGAImage& image)
    {
        image.read_tga_file(img_name);
    }
};

int main(int argc, const char* argv[])
{
    Image image = TGAImage{ 1600, 1600, TGAImage::RGB };
    Image texture = TGAImage{};
    std::visit(ImageReader{ "african_head_diffuse.tga" }, texture);

    auto objreader = tinyobj::ObjReader{};
    objreader.ParseFromFile("head.obj");

    if (!objreader.Valid())
    {
        std::cerr << "Invalid obj file, aborting...";
        return -1;
    }

    const auto& head = objreader.GetShapes().front();
    const auto& indices = head.mesh.indices;
    const auto& face_vertices = head.mesh.num_face_vertices;
    const auto& vertices = objreader.GetAttrib().vertices;
    const auto& tex_coords = objreader.GetAttrib().texcoords;

    const auto get_vertex = [&vertices](const auto idx) {
        const auto start_idx = idx * 3;
        return vec3f{ vertices[start_idx], vertices[start_idx + 1], vertices[start_idx + 2] };
    };

    const auto get_texcoords = [&tex_coords](const auto idx) {
        const auto start_idx = idx * 2;
        return vec2f{ tex_coords[start_idx], tex_coords[start_idx + 1]};
    };

    const auto triangle_to_screen_coords =
        [width = image.get_width(), height = image.get_height()](const auto&... v) -> Triangle {
        const auto calc_img_coord = [](const auto obj_coord, const auto image_dimension) {
            return static_cast<int>((obj_coord + 1.f) * image_dimension / 2.f + .5f);
        };

        return {
            vec3f{ static_cast<float>(calc_img_coord(get_x(v), width)),
                   static_cast<float>(calc_img_coord(get_y(v), height)),
                   get_z(v) }...
        };
    };

    const auto light_intensity = [light_vector = vec3f{ 0,0,-1 }](const auto& v0, const auto& v1, const auto& v2)
    {
        const auto n = cross(v2 - v0, v1 - v0);
        const auto l = n.norm();
        const auto normalized = normalize(n);

        return dot(light_vector, normalized);
    };

    size_t indices_idx = 0;
    std::vector<float> z_buffer(image.get_width()*image.get_height(), -std::numeric_limits<float>::max());
    for (size_t i = 0; i < face_vertices.size(); ++i)
    {
        const auto face_size = face_vertices[i];
        if (face_size != 3)
        {
            std::cerr << "Invalid obj file (face not triangle), aborting...";
            return -1;
        }

        const auto face = indices.begin() + indices_idx;
        const auto v0 = get_vertex(face[0].vertex_index);
        const auto v1 = get_vertex(face[1].vertex_index);
        const auto v2 = get_vertex(face[2].vertex_index);

        const auto t0 = get_texcoords(face[0].texcoord_index);
        const auto t1 = get_texcoords(face[1].texcoord_index);
        const auto t2 = get_texcoords(face[2].texcoord_index);

        const auto intensity = light_intensity(v0, v1, v2);
        if (intensity > 0)
        {
            draw_triangle(triangle_to_screen_coords(v0, v1, v2),
                {t0, t1, t2}, intensity, z_buffer, image, texture);
        }
        indices_idx += face_size;
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");

    return 0;
}
