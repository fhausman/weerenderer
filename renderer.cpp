#include "img/tgaimage.h"
#include "obj/obj.hpp"
#include "hola/hola.hpp"

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
    const int width = 1600;
    const int height = 1600;
    TGAImage image{width, height, TGAImage::RGB};

    draw_line({ 100, 100 }, { 1500, 1000 }, image, green);

    image.flip_vertically();
    image.write_tga_file("output.tga");

    return 0;
}
