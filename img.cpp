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
