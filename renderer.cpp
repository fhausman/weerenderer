#include "img/tgaimage.h"

void draw_line(const int x0, const int y0, const int x1, const int y1, TGAImage& image, const TGAColor color = TGAColor{ 255, 255, 255 , 255 })
{
    for (float t = 0.; t < 1.; t += 0.01)
    {
        auto x = x0 + (x1 - x0)*t;
        auto y = y0 + (y1 - y0)*t;

        image.set(x, y, color);
    }
}

int main(int argc, const char* argv[])
{
    TGAImage image{ 800, 800, TGAImage::RGB };

    draw_line(10, 10, 90, 100, image, TGAColor{ 255, 0, 0, 255 });

    image.flip_vertically();
    image.write_tga_file("output.tga");

    return 0;
}
