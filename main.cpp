#include "renderer.hpp"
#include "img.hpp"
#include "tgaimpl.hpp"
#include "model.hpp"
#include "objimpl.hpp"
#include "hola/hola.hpp"

#include <iostream>

using namespace hola;

using ImgPtr = std::unique_ptr<IImg>;
using ColorPtr = std::unique_ptr<IColor>;
using ModelPtr = std::unique_ptr<IModel>;

int main(int argc, const char* argv[])
{
    ImgPtr out_image = std::make_unique<TgaImage>();
    out_image->CreateImage(1600, 1600);

    ImgPtr texture = std::make_unique<TgaImage>();
    texture->ReadImage("african_head_diffuse.tga");

    ModelPtr model = std::make_unique<Obj>();
    model->ReadModel("head.obj");

    const auto[width, height] = out_image->GetImageSize();
    const auto triangle_to_screen_coords =
        [width, height](const auto&... v) -> Triangle {
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
        const auto normalized = normalize(n);

        return dot(light_vector, normalized);
    };

    size_t indices_idx = 0;
    std::vector<float> z_buffer(width*height, -std::numeric_limits<float>::max());
    while (const auto shape = model->GetNextShape())
    {
        while (const auto& polygon = shape->GetNextPolygon())
        {
            const auto& [v0, v1, v2] = polygon->vertices;
            const auto& [t0, t1, t2] = polygon->textureCoordinates;

            const auto intensity = light_intensity(v0, v1, v2);
            if (intensity > 0)
            {
                draw_triangle(triangle_to_screen_coords(v0, v1, v2),
                    { t0, t1, t2 }, intensity, z_buffer, *out_image, *texture);
            }
        }
    }
    out_image->WriteImage("output.tga");

    return 0;
}
