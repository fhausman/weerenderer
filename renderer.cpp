#include "img/tgaimage.h"
#include "obj/obj.hpp"
#include "geometry/vector.hpp"

#include <algorithm>
#include <array>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
const TGAColor green = TGAColor(0, 255, 0, 255);
const TGAColor blue = TGAColor(0, 0, 255, 255);

struct Vec2i
{
    int x;
    int y;
};

struct Triangle
{
    static size_t const vertices_num = 3;

    std::array<Vec2i, vertices_num> vertices;
    Triangle(const Vec2i& v0, const Vec2i& v1, const Vec2i& v2)
    {
        vertices = {v0, v1, v2};
        std::sort(vertices.begin(), vertices.end(),
            [](const auto& v0, const auto& v1) { return v0.y > v1.y; });
    }
};

void draw_line(const Vec2i& v0, const Vec2i& v1, TGAImage& image,
    const TGAColor color = TGAColor{255, 255, 255, 255})
{
    const auto dx = std::abs(v1.x - v0.x);
    const auto dy = std::abs(v1.y - v0.y);
    const auto step = dx >= dy ? dx : dy;

    const auto xi = v0.x > v1.x ? -1 : 1;
    const auto yi = v0.y > v1.y ? -1 : 1;
    for (int i = 0; i < step; ++i)
    {
        const auto calculate_next = [step, i](const auto start,
                                        const auto difference) {
            return (int)(start + i * (difference / (float)(step)));
        };

        const auto x = calculate_next(v0.x, dx * xi);
        const auto y = calculate_next(v0.y, dy * yi);
        image.set(x, y, color);
    }
}

void draw_triangle(const Triangle& triangle, TGAImage& image,
    const TGAColor color = TGAColor{255, 255, 255, 255})
{
    for (size_t i = 0; i < Triangle::vertices_num; ++i)
    {
        draw_line(triangle.vertices[i],
            triangle.vertices[(i + 1) % Triangle::vertices_num], image, color);
    }
}

int main(int argc, const char* argv[])
{
    const int width = 1600;
    const int height = 1600;
    TGAImage image{width, height, TGAImage::RGB};

    using namespace geo::vector::_vector;

    geo::vector::Vec2i vec{ 0, 2 }, vec1{ 1, 3 };

    auto vec_p = VecPair{ geo::vector::Vec2i{0, 1}, geo::vector::Vec2i{3, 4} };
    auto it = vec_p.begin();
    ++it;

    // auto model = Obj::CreateObjModel("head.obj");
    // auto draw_face = [&model, &image](const FaceElements& face) {
    //    const auto& face_coords = face.face_element_coords;
    //    const auto size = face_coords.size();
    //    for (size_t i = 0; i < size; ++i)
    //    {
    //        const auto v0 =
    //            model.GetVertexIndicesAt(face_coords[i].vertex_indices);
    //        const auto v1 = model.GetVertexIndicesAt(
    //            face_coords[(i + 1) % size].vertex_indices);
    //        int x0 = (int)((v0.x + 1.) * image.get_width() / 2.);
    //        int y0 = (int)((v0.y + 1.) * image.get_height() / 2.);
    //        int x1 = (int)((v1.x + 1.) * image.get_width() / 2.);
    //        int y1 = (int)((v1.y + 1.) * image.get_height() / 2.);
    //        draw_line({x0, y0}, {x1, y1}, image);
    //    }
    //};

    // const auto& faces = model.GetFaceElements();
    // std::for_each(faces.begin(), faces.end(), draw_face);

    draw_triangle({Vec2i{10, 70}, Vec2i{50, 160}, Vec2i{70, 80}}, image, red);
    draw_triangle(
        {Vec2i{180, 50}, Vec2i{150, 1}, Vec2i{70, 180}}, image, green);
    draw_triangle(
        {Vec2i{180, 150}, Vec2i{120, 160}, Vec2i{130, 180}}, image, blue);

    image.flip_vertically();
    image.write_tga_file("output.tga");

    return 0;
}
