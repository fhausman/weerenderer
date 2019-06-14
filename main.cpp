#include "renderer.hpp"
#include "img/tgaimage.h"
#include "tinyobjloader/tiny_obj_loader.h"
#include "hola/hola.hpp"

#include <iostream>
#include <cstdlib>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

using namespace hola;

bool is_inside_triangle(const vec3f& barycentric)
{
    return !(get_x(barycentric) < 0.f ||
        get_y(barycentric) < 0.f ||
        get_z(barycentric) < 0.f);
}

TGAColor get_color(const vec3f& barycentric, const std::array<vec2f, 3>& texture_coords, TGAImage& texture)
{
    const auto p_uv =
        texture_coords[0]*barycentric[0] +
        texture_coords[1]*barycentric[1] +
        texture_coords[2]*barycentric[2];

    const auto tex_x = texture.get_width() - get_x(p_uv)*texture.get_width();
    const auto tex_y = texture.get_height() - get_y(p_uv)*texture.get_height();
    return texture.get(tex_x, tex_y);
}

void draw_triangle(const Triangle& triangle, const std::array<vec2f, 3>& texture_coords, const float_t intensity, std::vector<float>& z_buffer, TGAImage& image, TGAImage& texture)
{
    const auto buffer_idx = [&](const auto& x, const auto& y) {
        return size_t(x + y * image.get_width());
    };

    const auto bbox = calculate_bounding_box(
        triangle, {image.get_width(), image.get_height()});

    for (auto x = get_x(bbox.min); x <= get_x(bbox.max); ++x)
    {
        for(auto y = get_y(bbox.min); y <= get_y(bbox.max); ++y)
        {
            const auto barycentric = calculate_barycentric({ x,y,0.f }, triangle);
            if (barycentric)
            {
                if (!is_inside_triangle(*barycentric))
                    continue;
            
                float z = 0.f;
                for(size_t i = 0; i < triangle.size(); ++i)
                {
                    z += get_z(triangle[i])*barycentric->at(i);
                }
                if(z_buffer[buffer_idx(x,y)] < z)
                {
                    z_buffer[buffer_idx(x,y)] = z;
                    image.set(x, y, get_color(*barycentric, texture_coords, texture)*intensity);
                }
            }
        }
    }
}

int main(int argc, const char* argv[])
{
    TGAImage image{ 1600, 1600, TGAImage::RGB };
    TGAImage texture;
    texture.read_tga_file("african_head_diffuse.tga");

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

    const auto rand_color = []()
    {
        return TGAColor(rand() % 255, rand() % 255, rand() % 255, 255);
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
