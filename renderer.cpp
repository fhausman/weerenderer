#include "img/tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

void draw_line(int x0, int y0, int x1, int y1, TGAImage& image, const TGAColor color = TGAColor{ 255, 255, 255 , 255 })
{
    for (auto x = x0; x <= x1; ++x)
    {
        int y = static_cast<int>(y0 + (x-x0) * ((y1 - y0) / static_cast<float>(x1 - x0)));
        image.set(x, y, color);
    }
}

int main(int argc, const char* argv[])
{
    TGAImage image{ 100, 100, TGAImage::RGB };

    draw_line(13, 20, 80, 40, image, white);
    draw_line(20, 13, 40, 80, image, red);
    draw_line(80, 40, 13, 20, image, red);

    image.flip_vertically();
    image.write_tga_file("output.tga");

    return 0;
}
