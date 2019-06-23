#include "img.hpp"

struct ImageReader
{
    const char* img_name;

    void operator()(TGAImage& image)
    {
        image.read_tga_file(img_name);
    }
};

struct ImageWriter
{
    const char* out_name;

    void operator()(TGAImage& image)
    {
        image.flip_vertically();
        image.write_tga_file(out_name);
    }
};

struct SetPixel
{
    const int32_t x;
    const int32_t y;
    const Color& color;
    const float_t intensity = 1;

    void operator()(TGAImage& image)
    {
        image.set(x, y, std::get<TGAColor>(color)*intensity);
    }
};

struct GetPixel
{
    const int32_t x;
    const int32_t y;

    Color operator()(TGAImage& image)
    {
        return image.get(x, y);
    }
};

struct ImageSize
{
    std::tuple<Width, Height> operator()(TGAImage& image)
    {
        return { image.get_width(), image.get_height() };
    }
};

std::tuple<Width, Height> get_image_size(Image& image)
{
    return std::visit(ImageSize{}, image);
}

void set_pixel(Image& image, const int32_t x, const int32_t y, const float_t intensity, const Color& color)
{
    return std::visit(SetPixel{ x,y,color,intensity }, image);
}

Color get_pixel(Image& image, const int32_t x, const int32_t y)
{
    return std::visit(GetPixel{ x,y }, image);
}

Image read_image(const std::filesystem::path& path_to_img)
{
    Image img;
    const auto extension = path_to_img.extension();
    if (extension != ".tga")
    {
        throw std::runtime_error("unsupported file");
    }
    else
    {
        img = TGAImage{};
    }

    std::visit(ImageReader{ path_to_img.string().c_str() }, img);
    return img;
}

void write_image(Image& out_image, const std::filesystem::path& path_to_write)
{
    std::visit(ImageWriter{ path_to_write.string().c_str() }, out_image);
}
