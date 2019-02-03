#include "img/tgaimage.h"
#include "obj/obj.hpp"

#include <algorithm>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void draw_line(const int x0, const int y0, const int x1, const int y1,
    TGAImage& image, const TGAColor color = TGAColor{255, 255, 255, 255})
{
    const auto dx = std::abs(x1 - x0);
    const auto dy = std::abs(y1 - y0);
    const auto step = dx >= dy ? dx : dy;

    const auto xi = x0 > x1 ? -1 : 1;
    const auto yi = y0 > y1 ? -1 : 1;
    for (int i = 0; i < step; ++i)
    {
        const auto calculate_next = [step, i](const auto start,
                                        const auto difference) {
            return (int)(start + i * (difference / (float)(step)));
        };

        const auto x = calculate_next(x0, dx*xi);
        const auto y = calculate_next(y0, dy*yi);
        image.set(x, y, color);
    }
}

int main(int argc, const char* argv[])
{
    const int width = 800;
    const int height = 800;
    TGAImage image{width, height, TGAImage::RGB};

    auto model = Obj::CreateObjModel(
        "C:\\Projects\\Repositories\\renderer\\cmake_Vs\\Debug\\head.obj");
    auto draw_face = [&model, &image](const FaceElements& face) {
        const auto& face_coords = face.face_element_coords;
        const auto size = face_coords.size();
        for (size_t i = 0; i < size; ++i)
        {
            const auto v0 =
                model.GetVertexIndicesAt(face_coords[i].vertex_indices);
            const auto v1 = model.GetVertexIndicesAt(
                face_coords[(i + 1) % size].vertex_indices);
            int x0 = (int)((v0.x + 1.) * image.get_width() / 2.);
            int y0 = (int)((v0.y + 1.) * image.get_height() / 2.);
            int x1 = (int)((v1.x + 1.) * image.get_width() / 2.);
            int y1 = (int)((v1.y + 1.) * image.get_height() / 2.);
            draw_line(x0, y0, x1, y1, image);
        }
    };

    const auto& faces = model.GetFaceElements();
    std::for_each(faces.begin(), faces.end(), draw_face);

    // for (int i = 0; i < model->nfaces(); i++) {
    //    std::vector<int> face = model->face(i);
    //    for (int j = 0; j < 3; j++) {
    //        Vec3f v0 = model->vert(face[j]);
    //        Vec3f v1 = model->vert(face[(j + 1) % 3]);
    //        int x0 = (v0.x + 1.)*width / 2.;
    //        int y0 = (v0.y + 1.)*height / 2.;
    //        int x1 = (v1.x + 1.)*width / 2.;
    //        int y1 = (v1.y + 1.)*height / 2.;
    //        line(x0, y0, x1, y1, image, white);
    //    }
    //}
    // TGAImage image{100, 100, TGAImage::RGB};

    // draw_line(13, 20, 80, 40, image, white);
    // draw_line(20, 13, 40, 80, image, red);
    // draw_line(80, 40, 13, 20, image, red);

     image.flip_vertically();
     image.write_tga_file("output.tga");

    return 0;
}
