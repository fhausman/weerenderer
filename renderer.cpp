#include "img/tgaimage.h"
#include "tinyobjloader/tiny_obj_loader.h"
#include "hola/hola.hpp"

#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

using namespace hola;

void draw_line(const vec2i& v0, const vec2i& v1, TGAImage& image,
    const TGAColor color = TGAColor{255, 255, 255, 255})
{
    const auto dx = std::abs(x(v1) - x(v0));
    const auto dy = std::abs(y(v1) - y(v0));
    const auto steps_nr = dx >= dy ? dx : dy;

    const auto xi = x(v0) > x(v1) ? -1 : 1;
    const auto yi = y(v0) > y(v1) ? -1 : 1;

    const auto calculate_next_pixel =
        [steps_nr](const auto start, const auto difference, const auto step) {
            return static_cast<int>(
                start + step * (difference / static_cast<float>(steps_nr)));
        };

    for (size_t step = 0; step < steps_nr; ++step)
    {
        const auto px_x = calculate_next_pixel(x(v0), dx * xi, step);
        const auto px_y = calculate_next_pixel(y(v0), dy * yi, step);
        image.set(px_x, px_y, color);
    }
}

int main(int argc, const char* argv[])
{
    TGAImage image{1600, 1600, TGAImage::RGB};

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
        return vec2f{ vertices[vertex_start], vertices[vertex_start + 1] };
    };

    const auto get_next_index = [](const auto face_idx, const auto face_size) {
        return (face_idx + 1) % face_size;
    };

    const auto calc_img_coord = [](const auto obj_coord, const auto image_dimension) {
        return static_cast<int>((obj_coord + 1.) * image_dimension / 2.);
    };

    size_t indices_idx = 0;
    for (size_t i = 0; i < face_vertices.size(); ++i)
    {
        const auto face_size = face_vertices[i];
        const auto face = std::vector<tinyobj::index_t>(indices.begin() + indices_idx, indices.begin() + indices_idx + face_size);
        for (size_t fi = 0; fi < face_size; ++fi)
        {
            const auto v0 = get_vertex(face[fi].vertex_index);
            const auto v1 = get_vertex(face[get_next_index(fi, face_size)].vertex_index);
            const auto p0 = vec2i{ calc_img_coord(x(v0), image.get_width()), calc_img_coord(y(v0), image.get_height())};
            const auto p1 = vec2i{ calc_img_coord(x(v1), image.get_width()), calc_img_coord(y(v1), image.get_height())};

            draw_line(p0, p1, image);
        }
        indices_idx += face_size;
    }

    image.flip_vertically();
    image.write_tga_file("output.tga");

    return 0;
}
