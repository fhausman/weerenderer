#include "img/tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void draw_line(const int x0, const int y0, const int x1, const int y1,
    TGAImage& image, const TGAColor color = TGAColor{255, 255, 255, 255})
{
    const auto dx = std::abs(x1 - x0);
    const auto dy = std::abs(y1 - y0);
    const auto step = dx >= dy ? dx : dy;

    const auto start_x = x0 < x1 ? x0 : x1;
    const auto start_y = y0 < y1 ? y0 : y1;
    for (int i = 0; i < step; ++i)
    {
        const auto x = (int)(start_x + i * (dx / (float)(step)));
        const auto y = (int)(start_y + i * (dy / (float)(step)));
        image.set(x, y, color);
    }
}

int main(int argc, const char* argv[])
{
    TGAImage image{100, 100, TGAImage::RGB};

    draw_line(13, 20, 80, 40, image, white);
    draw_line(20, 13, 40, 80, image, red);
    draw_line(80, 40, 13, 20, image, red);

    image.flip_vertically();
    image.write_tga_file("output.tga");

    return 0;
}
