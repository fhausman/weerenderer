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

void draw_line(const vec2i& v0, const vec2i& v1, TGAImage& image,
    const TGAColor color = TGAColor{ 255, 255, 255, 255 })
{
    const auto dx = std::abs(get_x(v1) - get_x(v0));
    const auto dy = std::abs(get_y(v1) - get_y(v0));
    const auto steps_nr = dx >= dy ? dx : dy;

    const auto xi = get_x(v0) > get_x(v1) ? -1 : 1;
    const auto yi = get_y(v0) > get_y(v1) ? -1 : 1;

    const auto calculate_next_pixel =
        [steps_nr](const auto start, const auto difference, const auto step) {
        return static_cast<int>(
            start + step * (difference / static_cast<float>(steps_nr)));
    };

    for (int step = 0; step < steps_nr; ++step)
    {
        const auto px_x = calculate_next_pixel(get_x(v0), dx * xi, step);
        const auto px_y = calculate_next_pixel(get_y(v0), dy * yi, step);
        image.set(px_x, px_y, color);
    }
}

bool is_inside_triangle(const vec3f& barycentric)
{
    return !(get_x(barycentric) < 0.f ||
        get_y(barycentric) < 0.f ||
        get_z(barycentric) < 0.f);
}

void draw_triangle(const Triangle& triangle, std::vector<float>& z_buffer, TGAImage& image, const TGAColor color)
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
                    image.set(x, y, color);
                }
            }
        }
    }
}

int main(int argc, const char* argv[])
{
    TGAImage image{ 1600, 1600, TGAImage::RGB };

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

    const auto get_vertex = [&vertices](const auto idx) {
        const auto vertex_start = idx * 3;
        return vec3f{ vertices[vertex_start], vertices[vertex_start + 1], vertices[vertex_start + 2] };
    };

    const auto get_next_index = [](const auto face_idx, const auto face_size) {
        return (face_idx + 1) % face_size;
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

        const auto intensity = light_intensity(v0, v1, v2);
        if (intensity > 0)
        {
            draw_triangle(triangle_to_screen_coords(v0, v1, v2), z_buffer, image,
                TGAColor(intensity * 255, intensity * 255, intensity * 255, 255));
        }
        indices_idx += face_size;
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");

    return 0;
}
